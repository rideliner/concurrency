// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/worker.hpp>
#include <ride/concurrency/detail/worker_factory.hpp>

namespace ride {

using Worker = detail::WorkerThread;

template <class Worker_>
using WorkerFactory = detail::SimpleWorkerThreadFactory<Worker_>;

} // end namespace ride

