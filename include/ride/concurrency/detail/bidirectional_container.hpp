// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/container.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class BidirectionalConcurrentContainer
  : public ConcurrentContainer<T_, Container_>
  , protected AbstractForwardContainer<T_, Container_>
  , protected AbstractBackwardContainer<T_, Container_>
{
    typedef typename ConcurrentContainer<T_, Container_>::LockPtr LockPtr;

    inline Container_& getInternalData() override
    { return this->data; }
  public:
    BidirectionalConcurrentContainer() = default;
    virtual ~BidirectionalConcurrentContainer() = default;

    void pushFront(const T_& element)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);
    }

    template <class Rep_, class Period_>
    bool tryPushFrontFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPushFrontUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);

        return true;
    }

    bool tryPushFront(const T_& element)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeAddFront(element);
        this->safeFinishAdd(lock);

        return true;
    }

    void pushFront(T_&& element)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);
    }

template <class Rep_, class Period_>
    bool tryPushFrontFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);
    }

    template <class Clock_, class Duration_>
    bool tryPushFrontUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    bool tryPushFront(T_&& element)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeAddFront(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_>
    void emplaceFront(Args_&&... args)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);
    }

    template <class... Args_, class Rep_, class Period_>
    bool tryEmplaceFrontFor(Args_&&... args, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeAddFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_, class Clock_, class Duration_>
    bool tryEmplaceFrontUntil(Args_&&... args, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_>
    bool tryEmplaceFront(Args_&&... args)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeCreateFront(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    void pushBack(const T_& element)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddBack(element);
        this->safeFinishAdd(lock);
    }

    template <class Rep_, class Period_>
    bool tryPushBackFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeAddBack(element);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPushBackUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeAddBack(element);
        this->safeFinishAdd(lock);

        return true;
    }

    bool tryPushBack(const T_& element)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeAddBack(element);
        this->safeFinishAdd(lock);

        return true;
    }

    void pushBack(T_&& element)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeAddBack(std::move(element));
        this->safeFinishAdd(lock);
    }

    template <class Rep_, class Period_>
    bool tryPushBackFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeAddBack(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPushBackUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeAddBack(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    bool tryPushBack(T_&& element)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeAddBack(std::move(element));
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_>
    void emplaceBack(Args_&&... args)
    {
        LockPtr lock;
        this->safeAdd(lock);
        this->unsafeCreateBack(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);
    }

    template <class... Args_, class Rep_, class Period_>
    bool tryEmplaceBackFor(Args_&&... args, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, duration))
            return false;

        this->unsafeCreateBack(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_, class Clock_, class Duration_>
    bool tryEmplaceBackUntil(Args_&&... args, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, timeout_time))
            return false;

        this->unsafeCreateBack(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    template <class... Args_>
    bool tryEmplaceBack(Args_&&... args)
    {
        LockPtr lock;
        if (!this->safeTryAdd(lock, std::try_to_lock))
            return false;

        this->unsafeCreateBack(std::forward<Args_>(args)...);
        this->safeFinishAdd(lock);

        return true;
    }

    void popFront(T_& element)
    {
        LockPtr lock;
        this->safeRemove(lock);
        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);
    }

    template <class Rep_, class Period_>
    bool tryPopFrontFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, duration))
            return false;

        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPopFrontUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, timeout_time))
            return false;

        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);

        return true;
    }

    bool tryPopFront(const T_& element)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, std::try_to_lock))
            return false;

        this->unsafeRemoveFront(element);
        this->safeFinishRemove(lock);

        return true;
    }

    void popFront(T_&& element)
    {
        LockPtr lock;
        this->safeRemove(lock);
        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);
    }

    template <class Rep_, class Period_>
    bool tryPopFrontFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, duration))
            return false;

        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPopFrontUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, timeout_time))
            return false;

        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    bool tryPopFront(T_&& element)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, std::try_to_lock))
            return false;

        this->unsafeRemoveFront(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    void popBack(T_& element)
    {
        LockPtr lock;
        this->safeRemove(lock);
        this->unsafeRemoveBack(element);
        this->safeFinishRemove(lock);
    }

    template <class Rep_, class Period_>
    bool tryPopBackFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, duration))
            return false;

        this->unsafeRemoveBack(element);
        this->safeFinishRemove(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPopBackUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, timeout_time))
            return false;

        this->unsafeRemoveBack(element);
        this->safeFinishRemove(lock);

        return true;
    }

    bool tryPopBack(const T_& element)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, std::try_to_lock))
            return false;

        this->unsafeRemoveBack(element);
        this->safeFinishRemove(lock);

        return true;
    }

    void popBack(T_&& element)
    {
        LockPtr lock;
        this->safeRemove(lock);
        this->unsafeRemoveBack(std::move(element));
        this->safeFinishRemove(lock);
    }

    template <class Rep_, class Period_>
    bool tryPopBackFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, duration))
            return false;

        this->unsafeRemoveBack(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    template <class Clock_, class Duration_>
    bool tryPopBackUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, timeout_time))
            return false;

        this->unsafeRemoveBack(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }

    bool tryPopBack(T_&& element)
    {
        LockPtr lock;
        if (!this->safeTryRemove(lock, std::try_to_lock))
            return false;

        this->unsafeRemoveBack(std::move(element));
        this->safeFinishRemove(lock);

        return true;
    }
};

} // end namespace detail

} // end namespace ride

