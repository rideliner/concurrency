// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <list>

#include <ride/concurrency/containers/detail/bidirectional_container.hpp>

namespace ride {

namespace detail {

template <class T_, class Alloc_, class... Args_>
class Emplacer<std::list<T_, Alloc_>, Args_...>
  : AbstractEmplacer<std::list<T_, Alloc_>>
  , BasicForwardEmplacer<Args_...>
  , BasicBackwardEmplacer<Args_...>
{
  public:
    using AbstractEmplacer<std::list<T_, Alloc_>>::AbstractEmplacer;

    inline void emplaceFront(Args_&&... args) override
    {
        this->container.emplace_front(std::forward<Args_>(args)...);
    }

    inline void emplaceBack(Args_&&... args) override
    {
        this->container.emplace_back(std::forward<Args_>(args)...);
    }
};

} // end namespace detail

template <class T_, class Alloc_ = std::allocator<T_>>
class ConcurrentList
  : public detail::BidirectionalConcurrentContainer<T_, std::list<T_, Alloc_>>
{
  private:
    inline bool unsafeIsEmpty() const override
    { return this->data.empty(); }

    inline std::size_t unsafeSize() const override
    { return this->data.size(); }

    inline void unsafeClear() override
    { this->data.clear(); }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
    inline void unsafeAddFront(const T_& element)
    { this->data.push_front(element); }
    inline void unsafeAddFront(T_&& element)
    { this->data.push_front(std::move(element)); }
    inline void unsafeAddBack(const T_& element)
    { this->data.push_back(element); }
    inline void unsafeAddBack(T_&& element)
    { this->data.push_back(std::move(element)); }

    inline void unsafeRemoveFront(T_& element)
    {
        element = this->data.front();
        this->data.pop_front();
    }

    inline void unsafeRemoveFront(T_&& element)
    {
        element = std::move(this->data.front());
        this->data.pop_front();
    }

    inline void unsafeRemoveBack(T_& element)
    {
        element = this->data.back();
        this->data.pop_back();
    }

    inline void unsafeRemoveBack(T_&& element)
    {
        element = std::move(this->data.back());
        this->data.pop_back();
    }
#pragma clang diagnostic pop
  public:
    using detail::BidirectionalConcurrentContainer<T_, std::list<T_, Alloc_>>::BidirectionalConcurrentContainer;
    virtual ~ConcurrentList() = default;
};

} // end namespace ride

