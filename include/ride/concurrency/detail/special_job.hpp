// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/abstract_job.hpp>
#include <ride/concurrency/detail/barrier.hpp>

namespace ride { namespace detail {

template <class Mutex_>
class BarrierJob
  : public AbstractJob
{
  protected:
    std::shared_ptr<Barrier<Mutex_>> barrier;
  public:
    BarrierJob() = delete;

    BarrierJob(std::shared_ptr<Barrier<Mutex_>> barrier)
      : barrier(barrier)
    { }
};

template <class Mutex_>
class SynchronizeJob
  : public BarrierJob<Mutex_>
{
  public:
    using BarrierJob<Mutex_>::BarrierJob;

    inline void operator()() override
    { this->barrier->unblockAndWait(); }

    inline bool isPoison() const override
    { return false; }

    inline bool isSync() const final override
    { return true; }
};

template <class Mutex_>
class PoisonJob
  : public BarrierJob<Mutex_>
{
  public:
    using BarrierJob<Mutex_>::BarrierJob;

    inline void operator()() override
    { if (this->barrier) this->barrier->unblock(); }

    inline bool isSync() const override
    { return false; }

    inline bool isPoison() const final override
    { return true; }
};

} // end namespace detail

} // end namespace ride

