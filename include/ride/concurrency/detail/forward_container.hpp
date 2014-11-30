
#pragma once

#include <ride/concurrency/detail/container.hpp>

namespace ride { namespace detail {

template <class T_, class Container_, bool LRef_ = LRef_t<T_>::value, bool RRef_ = RRef_t<T_>::value>
class ForwardConcurrentContainer
  : public ConcurrentContainer<T_, Container_>
  , public AbstractForwardContainer<T_, LRef_, RRef_>
{
  public:
    ForwardConcurrentContainer() = default;

    inline void push(const T_& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(element);
        });
    }

    template <class... Args_>
    inline bool tryPush(const T_& element, Args_... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(element);
        }, std::forward<Args_>(args)...);
    }

    inline void push(T_&& element)
    {
        this->safeAdd([&]() {
            this->unsafeAddFront(std::move(element));
        });
    }

    template <class... Args_>
    inline bool tryPush(T_&& element, Args_... args)
    {
        return this->safeTryAdd([&]() {
            this->unsafeAddFront(std::move(element));
        }, std::forward<Args_>(args)...);
    }

    inline void pop(T_& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(element);
        });
    }

    template <class... Args_>
    inline bool tryPop(T_& element, Args_... args)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(element);
        }, std::forward<Args_>(args)...);
    }

    inline void pop(T_&& element)
    {
        this->safeRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        });
    }

    template <class... Args_>
    inline bool tryPop(T_&& element, Args_... args)
    {
        return this->safeTryRemove([&]() {
            this->unsafeRemoveFront(std::move(element));
        }, std::forward<Args_>(args)...);
    }
};

} // end namespace detail

} // end namespace ride

