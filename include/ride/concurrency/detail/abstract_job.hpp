// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace ride { namespace detail {

class PoolWorkerKey;

class AbstractJob
{
  public:
    AbstractJob() = default;
    virtual ~AbstractJob() = default;

    virtual void operator()(const PoolWorkerKey&) = 0;
    virtual bool isPoison() const = 0;
    virtual bool isSync() const = 0;
};

} // end namespace detail

} // end namespace ride

