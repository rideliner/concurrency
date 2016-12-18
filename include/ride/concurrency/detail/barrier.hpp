// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <condition_variable>
#include <mutex>

namespace ride { namespace detail {

// this can be replaced by std::latch
class Barrier
{
    std::mutex mutex;
    std::atomic_size_t waiting_for;
    std::condition_variable cond;
  public:
    Barrier() = delete;
    Barrier(const Barrier&) = delete;
    Barrier& operator = (const Barrier&) = delete;
    virtual ~Barrier() = default;

    Barrier(std::size_t wait_for_occur)
      : waiting_for(wait_for_occur)
    { }

    inline void count_down()
    {
        std::lock_guard<std::mutex> lock(this->mutex);

        unsafeUnblock();
    }

    inline void count_down_and_wait()
    {
        std::unique_lock<std::mutex> lock(this->mutex);

        unsafeUnblock();
        unsafeWait(std::move(lock));
    }

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(this->mutex);

        unsafeWait(std::move(lock));
    }
  private:
    inline void unsafeUnblock()
    {
        if (--waiting_for == 0)
            cond.notify_all();
    }

    inline void unsafeWait(std::unique_lock<std::mutex>&& lock)
    {
        while (waiting_for != 0)
            cond.wait(lock);
        lock.unlock();
    }
};

} // end namespace detail

} // end namespace ride

