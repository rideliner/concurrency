// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace ride {

class ThreadPool;

namespace detail {

class WorkerThread;

class StartWorkerKey
{
    friend class ::ride::ThreadPool;

    StartWorkerKey() = default;
    virtual ~StartWorkerKey() = default;
};

class PoolWorkerKey
{
    friend class WorkerThread;

    PoolWorkerKey() = default;
    virtual ~PoolWorkerKey() = default;
};

} // end namespace detail

} // end namespace ride

