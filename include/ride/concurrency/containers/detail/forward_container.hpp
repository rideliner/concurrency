// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/containers/detail/container.hpp>
#include <ride/concurrency/containers/detail/forward_operations.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class ForwardConcurrentContainer
  : public ConcurrentContainer<T_, Container_>
  , public ForwardLRefOperations<T_>
  , public ForwardRRefOperations<T_>
  , public ForwardEmplaceOperations<Container_>
{
    inline Container_& getInternalData() override
    { return this->data; }
  public:
    using ConcurrentContainer<T_, Container_>::ConcurrentContainer;
    virtual ~ForwardConcurrentContainer() = default;
};

} // end namespace detail

} // end namespace ride
