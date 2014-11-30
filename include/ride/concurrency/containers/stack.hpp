
#pragma once

#include <stack>

#include <ride/concurrency/detail/forward_container.hpp>

namespace ride {

template <class T_>
class ConcurrentStack
  : public detail::ForwardConcurrentContainer<T_, std::stack<T_>>
{
  protected:
    bool unsafeIsEmpty() const override
    { return this->data.empty(); }

    std::size_t unsafeSize() const override
    { return this->data.size(); }

    void unsafeClear() override
    {
        for (std::size_t i = 0; i < this->unsafeSize(); ++i)
            this->data.pop();
    }

    inline void unsafeAddFront(const T_& element)
    { this->data.push(element); }
    inline void unsafeAddFront(T_&& element)
    { this->data.push(std::move(element));

    inline void unsafeRemoveFront(T_& element)
    {
        element = this->data.top();
        this->data.pop();
    }

    inline void unsafeRemoveFront(T_&& element)
    {
        element = std::move(this->data.top());
        this->data.pop();
    }
  public:
    ConcurrentStack() = default;
};

} // end namespace ride

