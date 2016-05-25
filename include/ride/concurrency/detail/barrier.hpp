
#pragma once

#include <condition_variable>
#include <mutex>

namespace ride { namespace detail {

template <class Mutex_>
class Barrier
{
    typedef std::unique_lock<Mutex_> Lock;

    Mutex_& mutex;
    std::atomic_size_t waiting_for;
    std::condition_variable_any cond;
  public:
    Barrier() = delete;
    Barrier(const Barrier&) = delete;
    Barrier& operator = (const Barrier&) = delete;
    virtual ~Barrier() { }

    Barrier(Mutex_& mutex, std::size_t wait_for_occur)
      : mutex(mutex)
      , waiting_for(wait_for_occur)
    { }

    inline void unblock()
    {
        Lock lock(this->mutex);

        unsafeUnblock();

        lock.unlock();
    }

    inline void unblockAndWait()
    {
        Lock lock(this->mutex);

        unsafeUnblock();
        unsafeWait(lock);

        lock.unlock();
    }

    inline void wait()
    {
        Lock lock(this->mutex);

        unsafeWait(lock);

        lock.unlock();
    }

    inline void unsafeUnblock()
    {
        if (--waiting_for == 0)
            cond.notify_all();
    }

    inline void unsafeWait(Lock& lock)
    {
        cond.wait(lock, [&]() { return waiting_for == 0; });
    }
};

} // end namespace detail

} // end namespace ride

