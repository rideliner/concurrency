// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <condition_variable>
#include <mutex>

namespace ride { namespace detail {

class Gate
{
    bool closed;
    std::mutex mutex;
    std::condition_variable cond;
  public:
    Gate()
      : closed(false)
    { }

    Gate(const Gate&) = delete;
    Gate& operator = (const Gate&) = delete;
    virtual ~Gate() = default;

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(this->mutex);

        while (closed)
            cond.wait(lock);
        lock.unlock();
    }

    inline void open()
    {
        std::lock_guard<std::mutex> lock(this->mutex);

        closed = false;
        cond.notify_all();
    }

    inline void close()
    {
        std::lock_guard<std::mutex> lock(this->mutex);

        closed = true;
    }
};

} // end namespace detail

} // end namespace ride
