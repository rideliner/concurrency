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
class BidirectionalLRefOperations
{
  public:
    BidirectionalLRefOperations() = default;
    virtual ~BidirectionalLRefOperations() = default;
};

template <class T_>
class BidirectionalLRefOperations<T_, std::enable_if_t<LRef_v<T_>>>
  : protected AbstractForwardContainerLValRef<T_>
  , protected AbstractBackwardContainerLValRef<T_>
  , virtual private SafeConcurrentContainer
{
  public:
    BidirectionalLRefOperations() = default;
    virtual ~BidirectionalLRefOperations() = default;

    LRefAddOperation(pushFront, tryPushFront, unsafeAddFront)
    LRefRemoveOperation(popFront, tryPopFront, unsafeRemoveFront)
    LRefAddOperation(pushBack, tryPushBack, unsafeAddBack)
    LRefRemoveOperation(popBack, tryPopBack, unsafeRemoveBack)
};

template <class T_, class Enable_ = void>
class BidirectionalRRefOperations
{
  public:
    BidirectionalRRefOperations() = default;
    virtual ~BidirectionalRRefOperations() = default;
};

template <class T_>
class BidirectionalRRefOperations<T_, std::enable_if_t<RRef_v<T_>>>
  : protected AbstractForwardContainerRValRef<T_>
  , protected AbstractBackwardContainerRValRef<T_>
  , virtual private SafeConcurrentContainer
{
  public:
    BidirectionalRRefOperations() = default;
    virtual ~BidirectionalRRefOperations() = default;

    RRefAddOperation(pushFront, tryPushFront, unsafeAddFront)
    RRefRemoveOperation(popFront, tryPopFront, unsafeRemoveFront)
    RRefAddOperation(pushBack, tryPushBack, unsafeAddBack)
    RRefRemoveOperation(popBack, tryPopBack, unsafeRemoveBack)
};

template <class T_>
class BidirectionalEmplaceOperations
  : protected ForwardContainerEmplace<T_>
  , protected BackwardContainerEmplace<T_>
  , virtual private SafeConcurrentContainer
{
  public:
    BidirectionalEmplaceOperations() = default;
    virtual ~BidirectionalEmplaceOperations() = default;

    CreateOperation(emplaceFront, tryEmplaceFront, unsafeCreateFront)
    CreateOperation(emplaceBack, tryEmplaceBack, unsafeCreateBack)
};

} // end namespace detail

} // end namespace ride