// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/abstract_job.hpp>

#include <functional>
#include <future>

namespace ride { namespace detail {

template <class F_>
class ActionJob
  : public AbstractJob
{
  public:
    typedef F_ ResultType;
    typedef std::function<F_()> FunctionType;
  protected:
    std::promise<ResultType> promise;
    FunctionType func;
  public:
    ActionJob() = delete;
    ActionJob(const ActionJob&) = delete;
    ActionJob& operator = (const ActionJob&) = delete;
    virtual ~ActionJob() = default;

    ActionJob(FunctionType func)
      : func(func)
    { }

    ActionJob(ActionJob&& other)
      : promise(std::move(other.promise))
      , func(std::move(other.func))
    { }

    ActionJob& operator = (ActionJob&& other)
    {
        promise = std::move(other.promise);
        func = std::move(other.func);

        return *this;
    }

    inline std::future<ResultType> getFuture()
    { return promise.get_future(); }

    inline bool isPoison() const final override
    { return false; }

    inline bool isSync() const final override
    { return false; }
};

} // end namespace detail

} // end namespace ride

