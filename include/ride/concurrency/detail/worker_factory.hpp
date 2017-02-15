// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#include <ride/concurrency/pool.hpp>

namespace ride {

namespace detail {

class WorkerThread;

class WorkerThreadFactory
{
  protected:
    template <class Worker_, class... Args_>
    std::shared_ptr<WorkerThread> createWithArgs(std::shared_ptr<ride::ThreadPool> owner, Args_&&... args)
    { return std::shared_ptr<WorkerThread>(new Worker_(owner, std::forward<Args_>(args)...)); }
  public:
    virtual std::shared_ptr<WorkerThread> create(std::shared_ptr<ride::ThreadPool> owner) = 0;

    WorkerThreadFactory() = default;
    virtual ~WorkerThreadFactory() = default;
};

template <class Worker_>
class SimpleWorkerThreadFactory final
  : public WorkerThreadFactory
{
  protected:
    inline std::shared_ptr<WorkerThread> create(std::shared_ptr<ride::ThreadPool> owner) override
    { return this->createWithArgs<Worker_>(owner); }
  public:
    SimpleWorkerThreadFactory() = default;
    virtual ~SimpleWorkerThreadFactory() = default;
};

} // end namespace detail

} // end namespace ride

