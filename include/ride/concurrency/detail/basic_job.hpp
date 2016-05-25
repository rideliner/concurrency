
#pragma once

#include <ride/concurrency/detail/abstract_job.hpp>

#include <functional>
#include <future>

namespace ride { namespace detail {

template <class F_>
class BasicJob
  : public AbstractJob
{
  public:
    typedef F_ ResultType;
    typedef std::function<F_()> FunctionType;
  protected:
    std::promise<ResultType> promise;
    FunctionType func;
  public:
    BasicJob() = delete;
    BasicJob(const BasicJob&) = delete;
    BasicJob& operator = (const BasicJob&) = delete;

    BasicJob(FunctionType func)
      : func(func)
    { }

    BasicJob(BasicJob&& other)
      : promise(std::move(other.promise))
      , func(std::move(other.func))
    { }

    BasicJob&& operator = (BasicJob&& other)
    {
        promise = std::move(other.promise);
        func = std::move(other.func);
    }

    inline std::future<ResultType> getFuture() const
    { return promise.get_future(); }

    inline bool isPoison() const final override
    { return false; }

    inline bool isSync() const final override
    { return false; }
};

} // end namespace detail

} // end namespace ride

