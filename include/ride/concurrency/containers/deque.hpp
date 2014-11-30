
#pragma once

#include <deque>

#include <ride/concurrency/detail/bidirectional_container.hpp>

namespace ride {

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

    void unsafeAddFront(const T_& element)
    { this->data.push_front(element);
    }
    void unsafeAddFront(T_&& element)
    { this->data.push_front(std::move(element)); }
    void unsafeAddBack(const T_& element)
    { this->data.push_back(element);
    }
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
  public:
    ConcurrentDeque() = default;
};

} // end namespace ride

