// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <queue>

#include <ride/concurrency/container/detail/forward_container.hpp>

namespace ride {

namespace detail {

template <class T_, class BaseContainer_, class... Args_>
class Emplacer<std::queue<T_, BaseContainer_>, Args_...>
  : AbstractEmplacer<std::queue<T_, BaseContainer_>>
  , BasicForwardEmplacer<Args_...>
{
  public:
    using AbstractEmplacer<std::queue<T_, BaseContainer_>>::AbstractEmplacer;

    inline void emplaceFront(Args_&&... args) override
    {
        this->container.emplace(std::forward<Args_>(args)...);
    }
};

} // end namespace detail

template <class T_, class Alloc_ = std::allocator<T_>>
class ConcurrentQueue
  : public detail::ForwardConcurrentContainer<T_, std::queue<T_, std::deque<T_, Alloc_>>>
{
  private:
    inline bool unsafeIsEmpty() const override
    { return this->data.empty(); }

    inline std::size_t unsafeSize() const override
    { return this->data.size(); }

    inline void unsafeClear() override
    {
        for (std::size_t i = 0; i < this->unsafeSize(); ++i)
            this->data.pop();
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
    inline void unsafeAddFront(const T_& element)
    { this->data.push(element); }
    inline void unsafeAddFront(T_&& element)
    { this->data.push(std::move(element)); }

    inline void unsafeRemoveFront(T_& element)
    {
        element = this->data.front();
        this->data.pop();
    }

    inline void unsafeRemoveFront(T_&& element)
    {
        element = std::move(this->data.front());
        this->data.pop();
    }
#pragma clang diagnostic pop
  public:
    using detail::ForwardConcurrentContainer<T_, std::queue<T_, std::deque<T_, Alloc_>>>::ForwardConcurrentContainer;
    virtual ~ConcurrentQueue() = default;
};

} // end namespace ride
