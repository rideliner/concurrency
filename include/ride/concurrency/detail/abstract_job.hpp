
#pragma once

namespace ride { namespace detail {

class AbstractJob
{
  public:
    virtual void operator()() = 0;
    virtual bool isPoison() const = 0;
};

} // end namespace detail

} // end namespace ride

