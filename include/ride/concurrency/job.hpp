
#pragma once

#include <ride/concurrency/detail/basic_job.hpp>
#include <ride/concurrency/detail/special_job.hpp>

namespace ride {

template <class F_>
class Job
  : public detail::BasicJob<F_>
{
  public:
    Job(typename detail::BasicJob<F_>::FunctionType func)
      : detail::BasicJob<F_>(func)
    { }

    void operator ()() override
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
  : public detail::BasicJob<void>
{
  public:
    Job(typename detail::BasicJob<void>::FunctionType func)
      : detail::BasicJob<void>(func)
    { }

    void operator ()() override
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

