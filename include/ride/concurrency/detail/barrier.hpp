// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <condition_variable>
#include <mutex>

namespace ride { namespace detail {

template <class Mutex_>
class Barrier
{
    typedef std::unique_lock<Mutex_> Lock;
    typedef typename std::conditional<std::is_same<Mutex_, std::mutex>::value,
        std::condition_variable, std::condition_variable_any>::type ConditionVar;

    Mutex_& mutex;
    std::atomic_size_t waiting_for;
    ConditionVar cond;
  public:
    Barrier() = delete;
    Barrier(const Barrier&) = delete;
    Barrier& operator = (const Barrier&) = delete;
    virtual ~Barrier() = default;

    Barrier(Mutex_& mutex, std::size_t wait_for_occur)
      : mutex(mutex)
      , waiting_for(wait_for_occur)
    { }

    inline void unblock()
    {
        Lock lock(this->mutex);

        unsafeUnblock();

        lock.unlock();
    }

    inline void unblockAndWait()
    {
        Lock lock(this->mutex);

        unsafeUnblock();
        unsafeWait(lock);

        lock.unlock();
    }

    inline void wait()
    {
        Lock lock(this->mutex);

        unsafeWait(lock);

        lock.unlock();
    }

    inline void unsafeUnblock()
    {
        if (--waiting_for == 0)
            cond.notify_all();
    }

    inline void unsafeWait(Lock& lock)
    {
        while (waiting_for != 0)
            cond.wait(lock);
    }
};

} // end namespace detail

} // end namespace ride

