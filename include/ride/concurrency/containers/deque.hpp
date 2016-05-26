// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <deque>

#include <ride/concurrency/detail/bidirectional_container.hpp>

namespace ride {

namespace detail {

template <class T_, class... Args_>
class Emplacer<std::deque<T_>, Args_...>
  : AbstractEmplacer<std::deque<T_>>
  , BasicForwardEmplacer<std::deque<T_>, Args_...>
  , BasicBackwardEmplacer<std::deque<T_>, Args_...>
{
  public:
    using AbstractEmplacer<std::deque<T_>>::AbstractEmplacer;

    void emplaceFront(Args_&&... args) override
    {
        this->container.emplace_front(std::forward<Args_>(args)...);
    }

    void emplaceBack(Args_&&... args) override
    {
        this->container.emplace_back(std::forward<Args_>(args)...);
    }
};

} // end namespace detail

template <class T_>
class ConcurrentDeque
  : public detail::BidirectionalConcurrentContainer<T_, std::deque<T_>>
{
  protected:
    bool unsafeIsEmpty() const override
    { return this->data.empty(); }

    std::size_t unsafeSize() const override
    { return this->data.size(); }

    void unsafeClear() override
    { this->data.clear(); }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
    void unsafeAddFront(const T_& element)
    { this->data.push_front(element); }
    void unsafeAddFront(T_&& element)
    { this->data.push_front(std::move(element)); }
    void unsafeAddBack(const T_& element)
    { this->data.push_back(element); }
    void unsafeAddBack(T_&& element)
    { this->data.push_back(std::move(element)); }

    void unsafeRemoveFront(T_& element)
    {
        element = this->data.front();
        this->data.pop_front();
    }

    void unsafeRemoveFront(T_&& element)
    {
        element = std::move(this->data.front());
        this->data.pop_front();
    }

    void unsafeRemoveBack(T_& element)
    {
        element = this->data.back();
        this->data.pop_back();
    }

    void unsafeRemoveBack(T_&& element)
    {
        element = std::move(this->data.back());
        this->data.pop_back();
    }
#pragma clang diagnostic pop
  public:
    ConcurrentDeque() = default;
};

} // end namespace ride

