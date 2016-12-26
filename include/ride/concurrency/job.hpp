// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/action_job.hpp>
#include <ride/concurrency/detail/special_job.hpp>

namespace ride {

template <class F_>
class Job
  : public detail::ActionJob<F_>
{
  public:
    using detail::ActionJob<F_>::ActionJob;

    virtual ~Job() = default;

    void operator ()(const ride::detail::PoolWorkerKey&) override
    {
        try {
            this->promise.set_value(this->func());
        } catch (...) {
            this->promise.set_exception(std::current_exception());
        }
    }
};

template <>
class Job<void>
  : public detail::ActionJob<void>
{
  public:
    using detail::ActionJob<void>::ActionJob;

    virtual ~Job() = default;

    void operator ()(const ride::detail::PoolWorkerKey&) override
    {
        try {
            this->func();
            this->promise.set_value();
        } catch (...) {
            this->promise.set_exception(std::current_exception());
        }
    }
};

} // end namespace ride

