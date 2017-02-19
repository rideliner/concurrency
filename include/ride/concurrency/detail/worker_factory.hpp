// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

namespace ride { namespace detail {

class WorkerThread;
class ThreadPool;

class AbstractWorkerThreadFactory
{
  protected:
    template <class Worker_, class... Args_>
    std::shared_ptr<WorkerThread> createWithArgs(std::shared_ptr<ThreadPool> owner, Args_&&... args)
    { return std::shared_ptr<WorkerThread>(new Worker_(owner, std::forward<Args_>(args)...)); }
  public:
    virtual std::shared_ptr<WorkerThread> create(std::shared_ptr<ThreadPool> owner) = 0;

    AbstractWorkerThreadFactory() = default;
    virtual ~AbstractWorkerThreadFactory() = default;
};

template <class Worker_>
class WorkerThreadFactory final
  : public AbstractWorkerThreadFactory
{
  public:
    inline std::shared_ptr<WorkerThread> create(std::shared_ptr<ThreadPool> owner) override final
    { return this->createWithArgs<Worker_>(owner); }

    WorkerThreadFactory() = default;
    virtual ~WorkerThreadFactory() = default;
};

} // end namespace detail

} // end namespace ride

