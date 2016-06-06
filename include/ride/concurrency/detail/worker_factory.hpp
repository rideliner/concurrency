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
  public:
    virtual std::unique_ptr<WorkerThread> create(const CreateWorkerKey&, ride::ThreadPool& owner) const = 0;

    WorkerThreadFactory() = default;
    virtual ~WorkerThreadFactory() = default;
};

template <class Worker_>
class SimpleWorkerThreadFactory final
  : public WorkerThreadFactory
{
  public:
    std::unique_ptr<WorkerThread> create(const CreateWorkerKey& key, ride::ThreadPool& owner) const override
    {
        return std::unique_ptr<WorkerThread>(new Worker_(key, owner));
    }

    SimpleWorkerThreadFactory() = default;
    virtual ~SimpleWorkerThreadFactory() = default;
};

} // end namespace detail

} // end namespace ride

