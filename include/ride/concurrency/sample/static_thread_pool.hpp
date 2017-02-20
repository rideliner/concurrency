// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/thread_pool.hpp>

namespace ride {

class StaticThreadPool
  : public ride::ThreadPool
{
    // make operations to add/remove workers private
    using ride::ThreadPool::removeWorkers;
    using ride::ThreadPool::removeWorkersLater;
    using ride::ThreadPool::removeAllWorkers;
    using ride::ThreadPool::removeAllWorkersLater;
    using ride::ThreadPool::addWorkers;

    using ride::ThreadPool::ThreadPool;
  public:
    static std::shared_ptr<StaticThreadPool> create(std::size_t size, typename ride::ThreadPool::PolymorphicWorkerFactory factory)
    {
        std::shared_ptr<StaticThreadPool> pool(new StaticThreadPool());
        pool->addWorkers(size, factory);
        return pool;
    }
};

} // end namespace ride
