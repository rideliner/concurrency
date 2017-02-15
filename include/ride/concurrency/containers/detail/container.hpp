// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <memory>

#include <ride/concurrency/containers/detail/container_types.hpp>
#include <ride/concurrency/containers/detail/safe_container.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class ConcurrentContainer
  : virtual protected SafeConcurrentContainer
{
  public:
    typedef T_ Type;
    typedef Container_ ContainerType;
  protected:
    typedef std::timed_mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    typedef std::lock_guard<Mutex> LockGuard;

    ContainerType data;
  private:
    inline bool wait(Lock&, std::try_to_lock_t) const final
    { return !this->unsafeIsEmpty(); }
  protected:
    virtual bool unsafeIsEmpty() const = 0;
    virtual std::size_t unsafeSize() const = 0;
    virtual void unsafeClear() = 0;
  public:
    template <class... Args_>
    ConcurrentContainer(Args_&&... args)
      : data(args...)
    { }

    virtual ~ConcurrentContainer() = default;

    inline bool isEmpty() const
    {
        LockGuard lock(this->mutex);

        return this->unsafeIsEmpty();
    }

    inline std::size_t size() const
    {
        LockGuard lock(this->mutex);

        return this->unsafeSize();
    }

    inline void clear()
    {
        Lock lock(this->mutex);

        this->unsafeClear();

        lock.unlock();
    }
};

} // end namespace detail

} // end namespace ride

