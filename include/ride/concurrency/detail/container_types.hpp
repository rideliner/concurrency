
#pragma once

#include <type_traits>

#include "emplacer.hpp"

namespace ride { namespace detail {

template <class T_>
constexpr bool LRef_t = std::integral_constant<bool, std::is_copy_constructible<T_>::value && std::is_copy_assignable<T_>::value>::value;

template <class T_>
constexpr bool RRef_t = std::integral_constant<bool, std::is_move_constructible<T_>::value && std::is_move_assignable<T_>::value>::value;

template <class T_, bool Enabled_>
class AbstractForwardContainerLValRef;
template <class T_, bool Enabled_>
class AbstractForwardContainerRValRef;
template <class T_, bool Enabled_>
class AbstractBackwardContainerLValRef;
template <class T_, bool Enabled_>
class AbstractBackwardContainerRValRef;

template <class T_>
class AbstractForwardContainerLValRef<T_, false> { };

template <class T_>
class AbstractForwardContainerLValRef<T_, true>
{
  protected:
    virtual void unsafeAddFront(const T_&) = 0;
    virtual void unsafeRemoveFront(T_&) = 0;
};

template <class T_>
class AbstractForwardContainerRValRef<T_, false> { };

template <class T_>
class AbstractForwardContainerRValRef<T_, true>
{
  protected:
    virtual void unsafeAddFront(T_&&) = 0;
    virtual void unsafeRemoveFront(T_&&) = 0;
};

template <class T_>
class AbstractBackwardContainerLValRef<T_, false> { };

template <class T_>
class AbstractBackwardContainerLValRef<T_, true>
{
  protected:
    virtual void unsafeAddBack(const T_&) = 0;
    virtual void unsafeRemoveBack(T_&) = 0;
};

template <class T_>
class AbstractBackwardContainerRValRef<T_, false> { };

template <class T_>
class AbstractBackwardContainerRValRef<T_, true>
{
  protected:
    virtual void unsafeAddBack(T_&&) = 0;
    virtual void unsafeRemoveBack(T_&&) = 0;
};

template <class T_, class Container_>
class AbstractForwardContainer
  : protected AbstractForwardContainerLValRef<T_, LRef_t<T_>>
  , protected AbstractForwardContainerRValRef<T_, RRef_t<T_>>
  , protected ForwardContainerEmplace<Container_>
{ };

template <class T_, class Container_>
class AbstractBackwardContainer
  : protected AbstractBackwardContainerLValRef<T_, LRef_t<T_>>
  , protected AbstractBackwardContainerRValRef<T_, RRef_t<T_>>
  , protected BackwardContainerEmplace<Container_>
{ };

} // end namespace detail

} // end namespace ride

