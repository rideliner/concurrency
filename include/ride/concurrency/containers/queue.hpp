
#pragma once

#include <queue>

#include <ride/concurrency/detail/forward_container.hpp>

namespace ride {

template <class T_>
class ConcurrentQueue
  : public detail::ForwardConcurrentContainer<T_, std::queue<T_>>
{
  protected:
    bool unsafeIsEmpty() const
    { return this->data.empty(); }

    std::size_t unsafeSize() const
    { return this->data.size(); }

    void unsafeClear()
    {
        for (std::size_t i = 0; i < this->unsafeSize(); ++i)
            this->data.pop();
    }

    inline void unsafeAddFront(const T_& element)
    { this->data.push(element); }
    inline void unsafeAddFront(T_&& element)
    { this->data.push(std::move(element)); }

    inline void unsafeRemoveFront(T_& element)
    {
        element = this->data.front();
        this->data.pop();
    }

    inline void unsafeRemoveFront(T_&& element)
    {
        element = std::move(this->data.front());
        this->data.pop();
    }
  public:
    ConcurrentQueue() = default;
};

} // end namespace ride
