
#pragma once

#include <ride/concurrency/detail/container.hpp>

namespace ride { namespace detail {

template <class T_, class Container_, bool LRef_ = LRef_t<T_>::value, bool RRef_ = RRef_t<T_>::value>
class BidirectionalConcurrentContainer
  : public ConcurrentContainer<T_, Container_>
  , protected AbstractForwardContainer<T_, LRef_, RRef_>
  , protected AbstractBackwardContainer<T_, LRef_, RRef_>
{
  public:
    BidirectionalConcurrentContainer() = default;

    inline void pushFront(const T_& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(element);
        });
    }

    template <class... Args_>
    inline bool tryPushFront(const T_& element, Args_... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, std::forward<Args_>(args)...);
    }

    inline void pushFront(T_&& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(std::move(element));
        });
    }

    template <class... Args_>
    inline bool tryPushFront(T_&& element, Args_... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, std::forward<Args_>(args)...);
    }

    inline void pushBack(const T_& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddBack(element);
        });
    }

    template <class... Args_>
    inline bool tryPushBack(const T_& element, Args_... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(element);
        }, std::forward<Args_>(args)...);
    }

    inline void pushBack(T_&& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddBack(std::move(element));
        });
    }

    template <class... Args_>
    inline bool tryPushBack(T_&& element, Args_... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddBack(std::move(element));
        }, std::forward<Args_>(args)...);
    }

    inline void popFront(T_& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(element);
        });
    }

    template <class... Args_>
    inline bool tryPopFront(T_& element, Args_... args)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, std::forward<Args_>(args)...);
    }

    inline void popFront(T_&& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        });
    }

    template <class... Args_>
    inline bool tryPopFront(T_&& element, Args_... args)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, std::forward<Args_>(args)...);
    }

    inline void popBack(T_& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveBack(element);
        });
    }

    template <class... Args_>
    inline bool tryPopBack(T_& element, Args_... args)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(element);
        }, std::forward<Args_>(args)...);
    }

    inline void popBack(T_&& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveBack(std::move(element));
        });
    }

    template <class... Args_>
    inline bool tryPopBack(T_&& element, Args_... args)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveBack(std::move(element));
        }, std::forward<Args_>(args)...);
    }
};

} // end namespace detail

} // end namespace ride

