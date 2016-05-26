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
        this->condition.wait(lock, [this]() {
            return !this->unsafeIsEmpty();
        });
    }

    template <class Rep_, class Period_>
    inline bool wait(Lock& lock, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->condition.wait_for(lock, duration, [this]() {
            return !this->unsafeIsEmpty();
        });
    }

    template <class Clock_, class Duration_>
    inline bool wait(Lock& lock, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->condition.wait_until(lock, timeout_time, [this]() {
            return !this->unsafeIsEmpty();
        });
    }

    inline bool wait(Lock& lock, std::try_to_lock_t)
    {
        return this->condition.wait(lock, [this]() {
            return !this->unsafeIsEmpty();
        });
    }

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

    template <class Timeout_>
    inline bool tryLock(LockPtr& lock, Timeout_&& timeout) const
    {
        lock.reset(new Lock(this->mutex, std::forward<Timeout_>(timeout)));

        return resetLockIfNotOwned(lock);
    }

    template <class Timeout_>
    inline bool tryLockForRemove(LockPtr& lock, Timeout_&& timeout) const
    {
        return tryLock(lock, std::forward<Timeout_>(timeout)) && resetLockIfContainerEmpty(lock);
    }
  protected:
    template <class Action_>
    void safeAdd(Action_ action)
    {
        Lock lock(this->mutex);

        action();
        this->condition.notify_one();

        lock.unlock();
    }

    template <class Action_, class Timeout_>
    bool safeTryAdd(Action_ action, Timeout_&& timeout)
    {
        LockPtr lock;

        if (!tryLock(lock, std::forward<Timeout_>(timeout)))
            return false;

        action();
        this->condition.notify_one();

        lock->unlock();
        return true;
    }

    template <class Action_>
    void safeRemove(Action_ action)
    {
        Lock lock(this->mutex);

        this->wait(lock);

        action();

        lock.unlock();
    }

    template <class Action_, class Timeout_>
    bool safeTryRemove(Action_ action, Timeout_&& timeout)
    {
        LockPtr lock;

        if (!tryLockForRemove(lock, std::forward<Timeout_>(timeout)))
            return false;

        // if not empty or timedout
        if (!this->wait(lock, std::forward<Timeout_>(timeout)))
            return false;

        action();

        lock->unlock();

        return true;
    }

    virtual bool unsafeIsEmpty() const = 0;
    virtual std::size_t unsafeSize() const = 0;
    virtual void unsafeClear() = 0;
  public:
    ConcurrentContainer() = default;

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

