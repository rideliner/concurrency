// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <mutex>
#include <memory>
#include <condition_variable>

namespace ride { namespace detail {

class SafeConcurrentContainer
{
  protected:
    typedef std::timed_mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    typedef std::unique_ptr<Lock> LockPtr;

    mutable Mutex mutex;
  private:
    std::condition_variable_any condition;

    inline void wait(Lock& lock)
    {
        while (!wait(lock, std::try_to_lock))
            this->condition.wait(lock);
    }

    template <class Clock_, class Duration_>
    inline bool wait(Lock& lock, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        while (!wait(lock, std::try_to_lock))
            if (this->condition.wait_until(lock, timeout_time) == std::cv_status::timeout)
                return wait(lock, std::try_to_lock);
        return true;
    }

    virtual bool wait(Lock&, std::try_to_lock_t) const = 0;

    inline void obtainLock(LockPtr& lock) const
    { lock.reset(new Lock(this->mutex)); }

    static bool resetLockIfNotOwned(LockPtr& lock)
    {
        if (!lock->owns_lock())
        {
            lock.reset();
            return false;
        }

        return true;
    }

    template <class Timeout_>
    inline bool tryObtainLock(LockPtr& lock, Timeout_&& timeout) const
    {
        lock.reset(new Lock(this->mutex, std::forward<Timeout_>(timeout)));

        return resetLockIfNotOwned(lock);
    }
  public:
    inline void prepareSafeAdd(LockPtr& lock)
    { obtainLock(lock); }

    template <class Timeout_>
    inline bool prepareSafeTryAdd(LockPtr& lock, Timeout_&& timeout)
    { return tryObtainLock(lock, std::forward<Timeout_>(timeout)); }

    inline void prepareSafeRemove(LockPtr& lock)
    {
        obtainLock(lock);
        this->wait(*lock);
    }

    template <class Timeout_>
    inline bool prepareSafeTryRemove(LockPtr& lock, Timeout_&& timeout)
    {
        return tryObtainLock(lock, std::forward<Timeout_>(timeout))
                && this->wait(*lock, std::forward<Timeout_>(timeout));
    }

    template <class Rep_, class Period_>
    inline bool prepareSafeTryRemove(LockPtr& lock, const std::chrono::duration<Rep_, Period_>& duration)
    {
        // fix issue of double wait (obtain lock and wait for insertion)
        // converts the duration into a time_point object
        return prepareSafeTryRemove(lock, std::chrono::steady_clock::now() + duration);
    }

    inline void finishSafeAdd(LockPtr& lock)
    {
        this->condition.notify_one();
        lock->unlock();
    }

    inline void finishSafeRemove(LockPtr& lock)
    { lock->unlock(); }
};

} // end namespace detail

} // end namespace ride
