// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stack>

#include <ride/concurrency/detail/forward_container.hpp>

namespace ride {

namespace detail {

template <class T_, class... Args_>
class Emplacer<std::stack<T_>, Args_...>
  : AbstractEmplacer<std::stack<T_>>
  , BasicForwardEmplacer<std::stack<T_>, Args_...>
{
  public:
    using AbstractEmplacer<std::stack<T_>>::AbstractEmplacer;

    void emplaceFront(Args_&&... args) override
    {
        this->container.emplace(std::forward<Args_>(args)...);
    }
};

} // end namespace detail

template <class T_>
class ConcurrentStack
  : public detail::ForwardConcurrentContainer<T_, std::stack<T_>>
{
  protected:
    bool unsafeIsEmpty() const override
    { return this->data.empty(); }

    std::size_t unsafeSize() const override
    { return this->data.size(); }

    void unsafeClear() override
    {
        for (std::size_t i = 0; i < this->unsafeSize(); ++i)
            this->data.pop();
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
    inline void unsafeAddFront(const T_& element)
    { this->data.push(element); }
    inline void unsafeAddFront(T_&& element)
    { this->data.push(std::move(element));

    inline void unsafeRemoveFront(T_& element)
    {
        element = this->data.top();
        this->data.pop();
    }

    inline void unsafeRemoveFront(T_&& element)
    {
        element = std::move(this->data.top());
        this->data.pop();
    }
#pragma clang diagnostic pop
  public:
    ConcurrentStack() = default;
    virtual ~ConcurrentStack() = default;
};

} // end namespace ride

