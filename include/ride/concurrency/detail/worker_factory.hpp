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
    friend void ride::ThreadPool::addWorkers(std::size_t, ride::ThreadPool::PolymorphicWorkerFactory);
  protected:
    virtual std::unique_ptr<WorkerThread> create(ride::ThreadPool& owner) const = 0;
};

template <class Worker_>
class SimpleWorkerThreadFactory final
  : public WorkerThreadFactory
{
  protected:
    std::unique_ptr<WorkerThread> create(ride::ThreadPool& owner) const override
    {
        return std::unique_ptr<WorkerThread>(new Worker_(owner));
    }
};

} // end namespace detail

} // end namespace ride

