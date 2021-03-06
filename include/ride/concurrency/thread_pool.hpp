// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/pool.hpp>
#include <ride/concurrency/detail/worker.hpp>
#include <ride/concurrency/detail/worker_factory.hpp>

namespace ride {

template <class Func_>
using Job = detail::Job<Func_>;

using ThreadPool = detail::ThreadPool;

using WorkerThread = detail::WorkerThread;

template <class Worker_ = WorkerThread>
using WorkerThreadFactory = detail::WorkerThreadFactory<Worker_>;

} // end namespace ride
