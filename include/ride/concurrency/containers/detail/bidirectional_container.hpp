// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/containers/detail/container.hpp>
#include <ride/concurrency/containers/detail/bidirectional_operations.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class BidirectionalConcurrentContainer
  : public ConcurrentContainer<T_, Container_>
  , public BidirectionalLRefOperations<T_>
  , public BidirectionalRRefOperations<T_>
  , public BidirectionalEmplaceOperations<Container_>
{
    inline Container_& getInternalData() override
    { return this->data; }
  public:
    using ConcurrentContainer<T_, Container_>::ConcurrentContainer;
    virtual ~BidirectionalConcurrentContainer() = default;
};

} // end namespace detail

} // end namespace ride
