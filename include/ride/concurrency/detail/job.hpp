// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/action_job.hpp>
#include <ride/concurrency/detail/special_job.hpp>
#include <ride/concurrency/detail/job_traits.hpp>

namespace ride { namespace detail {

template <class F_>
class Job
  : public ActionJob<F_>
{
  public:
    using ActionJob<F_>::ActionJob;

    virtual ~Job() = default;

    inline void operator ()(const PoolWorkerKey&) override
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
  : public ActionJob<void>
{
  public:
    using ActionJob<void>::ActionJob;

    virtual ~Job() = default;

    inline void operator ()(const PoolWorkerKey&) override
    {
        try {
            this->func();
            this->promise.set_value();
        } catch (...) {
            this->promise.set_exception(std::current_exception());
        }
    }
};

} // end namespace detail

} // end namespace ride

