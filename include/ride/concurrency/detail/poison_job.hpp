
#pragma once

#include <ride/concurrency/detail/abstract_job.hpp>

namespace ride { namespace detail {

class PoisonJob
  : public AbstractJob
{
  public:
    inline void operator()() override { }

    inline bool isPoison() const final override
    { return true; }
};

} // end namespace detail

} // end namespace ride

