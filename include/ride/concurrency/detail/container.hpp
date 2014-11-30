
#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <memory>

#include <ride/concurrency/detail/container_types.hpp>

namespace ride { namespace detail {

template <class T_, class Container_>
class ConcurrentContainer
{
  public:
    typedef T_ Type;
    typedef Container_ ContainerType;
  protected:
    typedef std::mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    typedef std::lock_guard<Mutex> LockGuard;
    typedef std::unique_ptr<Lock> LockPtr;

    ContainerType data;
    mutable Mutex mutex;
    std::condition_variable condition;
  private:
    // returns false when wait timesouts
    inline bool wait(Lock& lock)
    {
        while (this->unsafeIsEmpty())
            this->condition.wait(lock);

        // return false since this method never times out
        return false;
    }

    template <class Rep_, class Period_>
    inline bool wait(Lock& lock, const std::chrono::duration<Rep_, Period_>& args)
    {
        bool timedout = false;

        while (this->unsafeIsEmpty() && !timedout)
            timedout = this->condition.wait_for(args) == std::cv_status::timeout;

        return timedout;
    }

    template <class Clock_, class Duration_>
    inline bool wait(Lock& lock, const std::chrono::time_point<Clock_, Duration_>& args)
    {
        bool timedout = false;

        while (this->unsafeIsEmpty() && !timedout)
            timedout = this->condition.wait_until(args) == std::cv_status::timeout;

        return timedout;
    }

    // returns true if the lock is still valid
    // returns false if the lock is reset
    static bool resetLockIfNotOwned(LockPtr& lock)
    {
        if (!lock->owns_lock())
        {
            lock.reset();
            return false;
        }

        return true;
    }

    bool resetLockIfContainerEmpty(LockPtr& lock) const
    {
        if (this->unsafeIsEmpty())
        {
            lock->unlock();
            lock.reset();

            return false;
        }

        return true;
    }

    template <class... Args_>
    inline bool tryLock(LockPtr& lock, Args_... args) const
    {
        lock.reset(new Lock(this->mutex, std::forward<Args_>(args)...));

        return resetLockIfNotOwned(lock);
    }

    inline bool tryLock(LockPtr& lock) const
    {
        return tryLock(lock, std::try_to_lock);
    }

    template <class... Args_>
    inline bool tryLockForRemove(LockPtr& lock, Args_... args) const
    {
        return tryLock(lock, std::forward<Args_>(args)...) && resetLockIfContainerEmpty(lock);
    }

    inline bool tryLockForRemove(LockPtr& lock) const
    {
        return tryLock(lock, std::try_to_lock);
    }
  protected:
    template <class A_>
    void safeAdd(A_ action)
    {
        Lock lock(this->mutex);

        action();
        this->condition.notify_one();

        lock.unlock();
    }

    template <class A_, class... Args_>
    bool safeTryAdd(A_ action, Args_... args)
    {
        LockPtr lock;

        if (!tryLock(lock, std::forward<Args_>(args)...))
            return false;

        action();
        this->condition.notify_one();

        lock->unlock();
        return true;
    }

    template <class A_>
    void safeRemove(A_ action)
    {
        Lock lock(this->mutex);

        this->wait(lock);

        action();

        lock.unlock();
    }

    template <class A_, class... Args_>
    bool safeTryRemove(A_ action, Args_... args)
    {
        LockPtr lock;

        if (!tryLockForRemove(lock, std::forward<Args_>(args)...))
            return false;

        // if not empty or timedout
        bool timedout = this->wait(lock, std::forward<Args_>(args)...);

        if (!timedout)
            action();

        lock->unlock();

        return !timedout;
    }

    virtual bool unsafeIsEmpty() const = 0;
    virtual std::size_t unsafeSize() const = 0;
    virtual void unsafeClear() = 0;
  public:
    ConcurrentContainer() = default;

    inline bool isEmpty() const
    {
        LockGuard lock(this->mutex);

        return this->unsafeIsEmpty();
    }

    inline std::size_t size() const
    {
        LockGuard lock(this->mutex);

        return this->unsafeSize();
    }

    inline void clear()
    {
        Lock lock(this->mutex);

        this->unsafeClear();

        lock.unlock();
    }
};

} // end namespace detail

} // end namespace ride

