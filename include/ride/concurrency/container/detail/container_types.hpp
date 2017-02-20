// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>

#include <ride/concurrency/container/detail/emplacer.hpp>

namespace ride { namespace detail {

template <class T_>
constexpr bool LRef_v = std::is_copy_assignable<T_>::value;

template <class T_>
constexpr bool RRef_v = std::is_move_assignable<T_>::value;

template <class T_>
class AbstractForwardContainerLValRef
{
  protected:
    virtual void unsafeAddFront(const T_&) = 0;
    virtual void unsafeRemoveFront(T_&) = 0;
};

template <class T_>
class AbstractForwardContainerRValRef
{
  protected:
    virtual void unsafeAddFront(T_&&) = 0;
    virtual void unsafeRemoveFront(T_&&) = 0;
};

template <class T_>
class AbstractBackwardContainerLValRef
{
  protected:
    virtual void unsafeAddBack(const T_&) = 0;
    virtual void unsafeRemoveBack(T_&) = 0;
};

template <class T_>
class AbstractBackwardContainerRValRef
{
  protected:
    virtual void unsafeAddBack(T_&&) = 0;
    virtual void unsafeRemoveBack(T_&&) = 0;
};

} // end namespace detail

} // end namespace ride

