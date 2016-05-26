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
    inline Container_& getInternalData() override
    { return this->data; }
  public:
    BidirectionalConcurrentContainer() = default;
    virtual ~BidirectionalConcurrentContainer() = default;

    inline void pushFront(const T_& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(element);
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPushFrontFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPushFrontUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, timeout_time);
    }

    inline bool tryPushFront(const T_& element)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, std::try_to_lock);
    }

    inline void pushFront(T_&& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(std::move(element));
        });
    }

template <class Rep_, class Period_>
    inline bool tryPushFrontFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPushFrontUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, timeout_time);
    }

    inline bool tryPushFront(T_&& element)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, std::try_to_lock);
    }

    template <class... Args_>
    inline void emplaceFront(Args_&&... args)
    {
        this->safeAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        });
    }

    template <class... Args_, class Rep_, class Period_>
    inline bool tryEmplaceFrontFor(Args_&&... args, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        }, duration);
    }

    template <class... Args_, class Clock_, class Duration_>
    inline bool tryEmplaceFrontUntil(Args_&&... args, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        }, timeout_time);
    }

    template <class... Args_>
    inline bool tryEmplaceFront(Args_&&... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateFront(std::forward<Args_>(args)...);
        }, std::try_to_lock);
    }

    inline void pushBack(const T_& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddBack(element);
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPushBackFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(element);
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPushBackUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(element);
        }, timeout_time);
    }

    inline bool tryPushBack(const T_& element)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(element);
        }, std::try_to_lock);
    }

    inline void pushBack(T_&& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddBack(std::move(element));
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPushBackFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(std::move(element));
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPushBackUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(std::move(element));
        }, timeout_time);
    }

    inline bool tryPushBack(T_&& element)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(std::move(element));
        }, std::try_to_lock);
    }

    template <class... Args_>
    inline void emplaceBack(Args_&&... args)
    {
        this->safeAdd([&]() {
            this->unsafeCreateBack(std::forward<Args_>(args)...);
        });
    }

    template <class... Args_, class Rep_, class Period_>
    inline bool tryEmplaceBackFor(Args_&&... args, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateBack(std::forward<Args_>(args)...);
        }, duration);
    }

    template <class... Args_, class Clock_, class Duration_>
    inline bool tryEmplaceBackUntil(Args_&&... args, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateBack(std::forward<Args_>(args)...);
        }, timeout_time);
    }

    template <class... Args_>
    inline bool tryEmplaceBack(Args_&&... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeCreateBack(std::forward<Args_>(args)...);
        }, std::try_to_lock);
    }

    inline void popFront(T_& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(element);
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPopFrontFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPopFrontUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, timeout_time);
    }

    inline bool tryPopFront(const T_& element)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, std::try_to_lock);
    }

    inline void popFront(T_&& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPopFrontFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPopFrontUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, timeout_time);
    }

    inline bool tryPopFront(T_&& element)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, std::try_to_lock);
    }

    inline void popBack(T_& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveBack(element);
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPopBackFor(const T_& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(element);
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPopBackUntil(const T_& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(element);
        }, timeout_time);
    }

    inline bool tryPopBack(const T_& element)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(element);
        }, std::try_to_lock);
    }

    inline void popBack(T_&& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveBack(std::move(element));
        });
    }

    template <class Rep_, class Period_>
    inline bool tryPopBackFor(T_&& element, const std::chrono::duration<Rep_, Period_>& duration)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(std::move(element));
        }, duration);
    }

    template <class Clock_, class Duration_>
    inline bool tryPopBackUntil(T_&& element, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(std::move(element));
        }, timeout_time);
    }

    inline bool tryPopBack(T_&& element)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(std::move(element));
        }, std::try_to_lock);
    }
};

} // end namespace detail

} // end namespace ride

