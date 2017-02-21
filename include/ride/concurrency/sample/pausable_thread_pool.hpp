// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/thread_pool.hpp>
#include <ride/concurrency/detail/gate.hpp>

namespace ride {

class PausableThreadPool
  : public ThreadPool
{
    detail::Gate pauser;
  protected:
    inline void beforeExecuteJob() override
    {
        pauser.wait();

        ThreadPool::beforeExecuteJob();
    }
  public:
    PausableThreadPool()
        : pauser()
    {}

    inline void pause()
    { this->pauser.close(); }

    inline void resume()
    { this->pauser.open(); }
};

} // end namespace ride
