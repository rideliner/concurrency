// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/container.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class ForwardConcurrentContainer
  : public ConcurrentContainer<T_, Container_>
  , public AbstractForwardContainer<T_, Container_>
{
    typedef typename ConcurrentContainer<T_, Container_>::LockPtr LockPtr;

    inline Container_& getInternalData() override
    { return this->data; }
  public:
    ForwardConcurrentContainer() = default;
    virtual ~ForwardConcurrentContainer() = default;

    void push(const T_& element)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);
    }

    template <class Rep_, class Period_>
    bool tryPushFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPushUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);

        return true;
    }

    bool tryPush(const T_& element)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);

        return true;
    }

    void push(T_&& element)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);
    }

    template <class Rep_, class Period_>
    bool tryPushFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPushUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    bool tryPush(T_&& element)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_>
    void emplace(Args_&&... args)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);
    }

    template <class... Args_, class Rep_, class Period_>
    bool tryEmplaceFor(Args_&&... args, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_, class Clock_, class Duration_>
    bool tryEmplaceUntil(Args_&&... args, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_>
    bool tryEmplace(Args_&&... args)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    void pop(T_& element)
    {
        LockPtr lock;
        this->safeRemove(lock);
        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);
    }

    template <class Rep_, class Period_>
    bool tryPopFor(T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, duration))
            return false;

        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPopUntil(T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, timeout_time))
            return false;

        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);

        return true;
    }

    bool tryPop(T_& element)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, std::try_to_lock))
            return false;

        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);

        return true;
    }

    void pop(T_&& element)
    {
        LockPtr lock;
        this->safeRemove(lock);
        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);
    }

    template <class Rep_, class Period_>
    bool tryPop(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, duration))
            return false;

        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPop(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, timeout_time))
            return false;

        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    bool tryPop(T_&& element)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, std::try_to_lock))
            return false;

        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }
};

} // end namespace detail

} // end namespace ride

