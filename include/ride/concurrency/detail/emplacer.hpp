
#pragma once

namespace ride { namespace detail {

// add specializations for each container
template <class Container_, class... Args_>
class Emplacer;

template <class Container_>
class AbstractEmplacer
{
  protected:
    Container_& container;
  public:
    AbstractEmplacer(Container_& container)
      : container(container)
    { }
};

template <class Container_, class... Args_>
class BasicForwardEmplacer
{
  public:
    virtual void emplaceFront(Args_&&... args) = 0;
};

template <class Container_, class... Args_>
class BasicBackwardEmplacer
{
  public:
    virtual void emplaceBack(Args_&&... args) = 0;
};

template <class Container_>
class ForwardContainerEmplace
{
  protected:
    virtual Container_& getInternalData() = 0;

    template <class... Args_>
    void unsafeCreateFront(Args_&&... args)
    {
        Emplacer<Container_, Args_...> emplace(getInternalData());
        emplace.emplaceFront(std::forward<Args_>(args)...);
    }
};

template <class Container_>
class BackwardContainerEmplace
{
  protected:
    virtual Container_& getInternalData() = 0;

    template <class... Args_>
    void unsafeCreateBack(Args_&&... args)
    {
        Emplacer<Container_, Args_...> emplace(getInternalData());
        emplace.emplaceBack(std::forward<Args_>(args)...);
    }
};

} // end namespace detail

} // end namespace ride
