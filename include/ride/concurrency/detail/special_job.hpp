
#pragma once

#include <ride/concurrency/detail/abstract_job.hpp>

namespace ride { namespace detail {

class SynchronizeJob
  : public AbstractJob
{
  public:
    inline void operator()() override { }

    inline bool isPoison() const override
    { return false; }

    inline bool isSync() const final override
    { return true; }
};

class PoisonJob
  : public SynchronizeJob
{
  public:
    inline bool isPoison() const final override
    { return true; }
};

} // end namespace detail

} // end namespace ride

