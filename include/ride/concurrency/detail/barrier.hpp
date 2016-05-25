
#pragma once

#include <condition_variable>
#include <mutex>

namespace ride { namespace detail {

class Barrier
{
    std::atomic_size_t waiting_for;
    std::condition_variable cond;
  public:
    Barrier() = delete;
    Barrier(const Barrier&) = delete;
    Barrier& operator = (const Barrier&) = delete;
    virtual ~Barrier() { }

    Barrier(std::size_t wait_for_occur)
      : waiting_for(wait_for_occur)
    { }

    inline void unblock()
    {
        if (--waiting_for == 0)
            cond.notify_all();
    }

    inline void unblockAndWait(std::unique_lock<std::mutex>& lock)
    {
        unblock();
        wait(lock);
    }

    // does not wait if the block is "opened"
    inline void wait(std::unique_lock<std::mutex>& lock)
    {
        while (waiting_for > 0)
            cond.wait(lock);
    }
};

} // end namespace detail

} // end namespace ride

