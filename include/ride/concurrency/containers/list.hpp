
#pragma once

#include <list>

#include <ride/concurrency/detail/bidirectional_container.hpp>

namespace ride {

template <class T_>
class ConcurrentList
  : public detail::BidirectionalConcurrentContainer<T_, std::list<T_>>
{
  protected:
    bool unsafeIsEmpty() const override
    { return this->data.empty(); }

    std::size_t unsafeSize() const override
    { return this->data.size(); }

    void unsafeClear() override
    { this->data.clear(); }

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
  public:
    ConcurrentList() = default;
};

} // end namespace ride

