// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/abstract_job.hpp>
#include <ride/concurrency/detail/barrier.hpp>

namespace ride { namespace detail {

class BarrierJob
  : public AbstractJob
{
  protected:
    std::shared_ptr<Barrier> barrier;
  public:
    BarrierJob() = delete;
    virtual ~BarrierJob() = default;

    BarrierJob(std::shared_ptr<Barrier> barrier)
      : barrier(barrier)
    { }
};

class SynchronizeJob
  : public BarrierJob
{
  public:
    using BarrierJob::BarrierJob;
    virtual ~SynchronizeJob() = default;

    inline void operator()() override
    { this->barrier->unblockAndWait(); }

    inline bool isPoison() const override
    { return false; }

    inline bool isSync() const final override
    { return true; }
};

class PoisonJob
  : public BarrierJob
{
  public:
    using BarrierJob::BarrierJob;
    virtual ~PoisonJob() = default;

    inline void operator()() override
    { if (this->barrier) this->barrier->unblock(); }

    inline bool isSync() const override
    { return false; }

    inline bool isPoison() const final override
    { return true; }
};

} // end namespace detail

} // end namespace ride

