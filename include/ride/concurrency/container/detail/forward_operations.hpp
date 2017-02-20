// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/container/detail/container_types.hpp>
#include <ride/concurrency/container/detail/safe_container.hpp>
#include <ride/concurrency/container/detail/operations.hpp>

namespace ride { namespace detail {

template <class T_, class Enable_ = void>
class ForwardLRefOperations
{
  public:
    ForwardLRefOperations() = default;
    virtual ~ForwardLRefOperations() = default;
};

template <class T_>
class ForwardLRefOperations<T_, std::enable_if_t<LRef_v<T_>>>
  : protected AbstractForwardContainerLValRef<T_>
  , virtual private SafeConcurrentContainer
{
  public:
    ForwardLRefOperations() = default;
    virtual ~ForwardLRefOperations() = default;

    LRefAddOperation(push, tryPush, unsafeAddFront)
    LRefRemoveOperation(pop, tryPop, unsafeRemoveFront)
};

template <class T_, class Enable_ = void>
class ForwardRRefOperations
{
  public:
    ForwardRRefOperations() = default;
    virtual ~ForwardRRefOperations() = default;
};

template <class T_>
class ForwardRRefOperations<T_, std::enable_if_t<RRef_v<T_>>>
  : protected AbstractForwardContainerRValRef<T_>
  , virtual private SafeConcurrentContainer
{
  public:
    ForwardRRefOperations() = default;
    virtual ~ForwardRRefOperations() = default;

    RRefAddOperation(push, tryPush, unsafeAddFront)
    RRefRemoveOperation(pop, tryPop, unsafeRemoveFront)
};

template <class T_>
class ForwardEmplaceOperations
  : protected ForwardContainerEmplace<T_>
  , virtual private SafeConcurrentContainer
{
  public:
    ForwardEmplaceOperations() = default;
    virtual ~ForwardEmplaceOperations() = default;

    CreateOperation(emplace, tryEmplace, unsafeCreateFront)
};

} // end namespace detail

} // end namespace ride