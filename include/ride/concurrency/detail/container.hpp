// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <memory>

#include <ride/concurrency/detail/container_types.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class ConcurrentContainer
{
  public:
    typedef T_ Type;
    typedef Container_ ContainerType;
  protected:
    typedef std::timed_mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    typedef std::lock_guard<Mutex> LockGuard;
    typedef std::unique_ptr<Lock> LockPtr;

    ContainerType data;
    mutable Mutex mutex;
    std::condition_variable_any condition;
  private:
    inline void wait(Lock& lock)
    {
        while (this->unsafeIsEmpty())
            this->condition.wait(lock);
    }

    template <class Clock_, class Duration_>
    inline bool wait(Lock& lock, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        while (this->unsafeIsEmpty())
            if (this->condition.wait_until(lock, timeout_time) == std::cv_status::timeout)
                return !this->unsafeIsEmpty();
        return true;
    }

    inline bool wait(Lock&, std::try_to_lock_t) const
    { return !this->unsafeIsEmpty(); }

    // returns true if the lock is still valid
    // returns false if the lock is reset
    static bool resetLockIfNotOwned(LockPtr& lock)
    {
        if (!lock->owns_lock())
        {
            lock.reset();
            return false;
        }

        return true;
    }

    bool resetLockIfContainerEmpty(LockPtr& lock) const
    {
        if (this->unsafeIsEmpty())
        {
            lock->unlock();
            lock.reset();

            return false;
        }

        return true;
    }

    inline void obtainLock(LockPtr& lock) const
    {
        lock.reset(new Lock(this->mutex));
    }

    template <class Timeout_>
    inline bool tryObtainLock(LockPtr& lock, Timeout_&& timeout) const
    {
        lock.reset(new Lock(this->mutex, std::forward<Timeout_>(timeout)));

        return resetLockIfNotOwned(lock);
    }

    template <class Timeout_>
    inline bool tryObtainLockForRemove(LockPtr& lock, Timeout_&& timeout) const
    {
        return tryObtainLock(lock, std::forward<Timeout_>(timeout))
               && resetLockIfContainerEmpty(lock);
    }
  protected:
    inline void safeAdd(LockPtr& lock)
    { obtainLock(lock); }

    template <class Timeout_>
    inline bool safeTryAdd(LockPtr& lock, Timeout_&& timeout)
    {
        return tryObtainLock(lock, std::forward<Timeout_>(timeout));
    }

    inline void safeFinishAdd(LockPtr& lock)
    {
        this->condition.notify_one();
        lock->unlock();
    }

    inline void safeRemove(LockPtr& lock)
    {
        obtainLock(lock);
        this->wait(*lock);
    }

    template <class Timeout_>
    inline bool safeTryRemove(LockPtr& lock, Timeout_&& timeout)
    {
        return tryObtainLockForRemove(lock, std::forward<Timeout_>(timeout))
               && this->wait(*lock, std::forward<Timeout_>(timeout));
    }

    template <class Rep_, class Period_>
    inline bool safeTryRemove(LockPtr& lock, const std::chrono::duration<Rep_, Period_>& duration)
    {
        // fix issue of double wait (obtain lock and wait for insertion)
        // converts the duration into a time_point object
        return safeTryRemove(lock, std::chrono::steady_clock::now() + duration);
    }

    inline void safeFinishRemove(LockPtr& lock)
    { lock->unlock(); }

    virtual bool unsafeIsEmpty() const = 0;
    virtual std::size_t unsafeSize() const = 0;
    virtual void unsafeClear() = 0;
  public:
    ConcurrentContainer() = default;
    virtual ~ConcurrentContainer() = default;

    inline bool isEmpty() const
    {
        LockGuard lock(this->mutex);

        return this->unsafeIsEmpty();
    }

    inline std::size_t size() const
    {
        LockGuard lock(this->mutex);

        return this->unsafeSize();
    }

    inline void clear()
    {
        Lock lock(this->mutex);

        this->unsafeClear();

        lock.unlock();
    }
};

} // end namespace detail

} // end namespace ride
