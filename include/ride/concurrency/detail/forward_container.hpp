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
    inline Container_& getInternalData() override
    { return this->data; }
  public:
    ForwardConcurrentContainer() = default;
    virtual ~ForwardConcurrentContainer() = default;

    inline void push(const T_& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(element);
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPushFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPushUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, timeout_time);
    }

    inline bool tryPush(const T_& element)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, std::try_to_lock);
    }

    inline void push(T_&& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(std::move(element));
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPushFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPushUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, timeout_time);
    }

    inline bool tryPush(T_&& element)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, std::try_to_lock);
    }

    template <class... Args_>
    inline void emplace(Args_&&... args)
    {
        this->safeAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        });
    }

    template <class... Args_, class Rep_, class Period_>
    inline bool tryEmplaceFor(Args_&&... args, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        }, duration);
    }

    template <class... Args_, class Clock_, class Duration_>
    inline bool tryEmplaceUntil(Args_&&... args, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        }, timeout_time);
    }

    template <class... Args_>
    inline bool tryEmplace(Args_&&... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        }, std::try_to_lock);
    }

    inline void pop(T_& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(element);
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPopFor(T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPopUntil(T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, timeout_time);
    }

    inline bool tryPop(T_& element)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, std::try_to_lock);
    }

    inline void pop(T_&& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPop(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPop(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, timeout_time);
    }

    inline bool tryPop(T_&& element)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, std::try_to_lock);
    }
};

} // end namespace detail

} // end namespace ride

