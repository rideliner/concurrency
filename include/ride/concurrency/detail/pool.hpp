// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <thread>
#include <unordered_map>

#include <ride/concurrency/detail/job.hpp>
#include <ride/concurrency/containers/deque.hpp>
#include <ride/concurrency/detail/pass_keys.hpp>

namespace ride { namespace detail {

class WorkerThread;
class AbstractWorkerThreadFactory;
class Barrier;

class ThreadPool
  : public std::enable_shared_from_this<ThreadPool>
{
  public:
    typedef std::unique_ptr<AbstractJob> PolymorphicJob;
    typedef ConcurrentDeque<PolymorphicJob> WorkContainer;
    typedef std::shared_ptr<WorkerThread> PolymorphicWorker;
    typedef std::shared_ptr<AbstractWorkerThreadFactory> PolymorphicWorkerFactory;
  private:
    typedef std::mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    typedef std::unique_ptr<Lock> LockPtr;
    typedef std::lock_guard<Mutex> LockGuard;

    WorkContainer work;
    mutable Mutex thread_management;
    StartWorkerKey starterKey;
    std::atomic_size_t num_pseudo_workers, num_alive_workers;
    std::shared_ptr<Barrier> join_barrier;
    std::unordered_map<std::thread::id, PolymorphicWorker> workers;

    std::pair<std::thread::id, PolymorphicWorker> createWorker(PolymorphicWorkerFactory factory);

    void unsafeAddWorkers(std::size_t to_create, PolymorphicWorkerFactory factory, LockPtr lock);

    std::size_t unsafeRemovePseudoWorkers(std::size_t to_remove, LockPtr lock);

    inline void unsafeRemoveWorkersNow(std::size_t to_remove, LockPtr lock)
    {
        to_remove = unsafeRemovePseudoWorkers(to_remove, std::move(lock));

        for (std::size_t i = 0; i < to_remove; ++i)
            this->work.pushFront(this->createPoisonPill(this->join_barrier));
    }

    inline void unsafeRemoveWorkersLater(std::size_t to_remove, LockPtr lock)
    {
        to_remove = unsafeRemovePseudoWorkers(to_remove, std::move(lock));

        for (std::size_t i = 0; i < to_remove; ++i)
            this->work.pushBack(this->createPoisonPill(this->join_barrier));
    }

    inline bool unsafeIsCurrentThreadInPool() const
    { return this->workers.find(std::this_thread::get_id()) != this->workers.end(); }

    void safeJoin(bool remove_workers);

    inline void synchronizeWorkers(std::size_t num_workers, std::shared_ptr<Barrier> barrier)
    {
        for (std::size_t i = 0; i < num_workers; ++i)
            this->work.pushBack(this->createSyncPill(barrier));
    }

    std::size_t setupBarrier(std::shared_ptr<Barrier>& barrier) const;

    static inline PolymorphicJob createPoisonPill(std::shared_ptr<Barrier> barrier)
    { return PolymorphicJob(new PoisonJob(barrier)); }

    static inline PolymorphicJob createSyncPill(std::shared_ptr<Barrier> barrier)
    { return PolymorphicJob(new SynchronizeJob(barrier)); }
  protected:
    inline virtual void afterExecuteJob(std::shared_ptr<WorkerThread> worker, AbstractJob& job) { }
    inline virtual void beforeExecuteJob(std::shared_ptr<WorkerThread> worker, AbstractJob& job) { }
    inline virtual void onStartupWorker(std::shared_ptr<WorkerThread> worker) { }
    inline virtual void onShutdownWorker(std::shared_ptr<WorkerThread> worker) { }
    inline virtual void onTimeoutWorker(std::shared_ptr<WorkerThread> worker) { }
    inline virtual void onSynchronizeWorker(std::shared_ptr<WorkerThread> worker) { }
  public:
    ThreadPool()
      : num_pseudo_workers(0)
      , num_alive_workers(0)
      , join_barrier(nullptr)
    { }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;

    template <class Func_, class Ret_ = typename JobResultType<Func_>::type>
    inline static std::unique_ptr<Job<Ret_>> createJob(Func_ function)
    { return std::unique_ptr<Job<Ret_>>(new Job<Ret_>(function)); }

    template <class Func_, class Ret_ = typename JobResultType<Func_>::type>
    inline std::future<Ret_> emplaceJob(Func_ function)
    {
        std::unique_ptr<Job<Ret_>> job = createJob(function);
        std::future<Ret_> future = job->getFuture();
        addJob(std::move(job));
        return future;
    }

    template <class Func_, class Ret_ = typename JobResultType<Func_>::type>
    inline std::future<Ret_> emplacePriorityJob(Func_ function)
    {
        std::unique_ptr<Job<Ret_>> job = createJob(function);
        std::future<Ret_> future = job->getFuture();
        addPriorityJob(std::move(job));
        return future;
    }

    template <class T_>
    inline void addJob(std::unique_ptr<Job<T_>>&& job_ptr)
    { this->work.pushBack(std::move(job_ptr)); }

    template <class T_>
    inline void addPriorityJob(std::unique_ptr<Job<T_>>&& job_ptr)
    { this->work.pushFront(std::move(job_ptr)); }

    inline void removeWorkers(std::size_t to_remove)
    {
        LockPtr lock(new Lock(this->thread_management));
        this->unsafeRemoveWorkersNow(to_remove, std::move(lock));
    }

    inline void removeWorkersLater(std::size_t to_remove)
    {
        LockPtr lock(new Lock(this->thread_management));
        this->unsafeRemoveWorkersLater(to_remove, std::move(lock));
    }

    inline void removeAllWorkers()
    {
        LockPtr lock(new Lock(this->thread_management));
        this->unsafeRemoveWorkersNow(this->numWorkers(), std::move(lock));
    }

    inline void removeAllWorkersLater()
    {
        LockPtr lock(new Lock(this->thread_management));
        this->unsafeRemoveWorkersLater(this->numWorkers(), std::move(lock));
    }

    inline void addWorkers(std::size_t to_create, PolymorphicWorkerFactory factory)
    {
        LockPtr lock(new Lock(this->thread_management));
        this->unsafeAddWorkers(to_create, factory, std::move(lock));
    }

    inline std::size_t numWorkers() const
    { return this->num_pseudo_workers; }
    inline std::size_t numAliveWorkers() const
    { return this->num_alive_workers; }
    inline std::size_t remainingJobs() const
    { return this->work.size(); }
    inline bool hasWork() const
    { return !this->work.isEmpty(); }
    inline void clearJobs()
    { this->work.clear(); }

    inline void join()
    { safeJoin(true); }
    inline void wait()
    { safeJoin(false); }
    void sync();

    inline bool isCurrentThreadInPool() const
    {
        LockGuard lock(this->thread_management);
        return unsafeIsCurrentThreadInPool();
    }
  public: // private key APIs
    inline void getJob(const PoolWorkerKey&, PolymorphicJob&& job)
    { this->work.popFront(std::move(job)); }

    inline bool tryGetJob(const PoolWorkerKey&, PolymorphicJob&& job, std::try_to_lock_t)
    { return this->work.tryPopFront(std::move(job)); }

    template <class Rep_, class Period_>
    inline bool tryGetJob(const PoolWorkerKey&, PolymorphicJob&& job, const std::chrono::duration<Rep_, Period_>& duration)
    { return this->work.tryPopFrontFor(std::move(job), duration); }

    template <class Clock_, class Duration_>
    inline bool tryGetJob(const PoolWorkerKey&, PolymorphicJob&& job, const std::chrono::time_point<Clock_, Duration_>& timeout_time)
    { return this->work.tryPopFrontUntil(std::move(job), timeout_time); }

    inline void handleAfterExecuteJob(const PoolWorkerKey&, std::shared_ptr<WorkerThread> worker, AbstractJob& job)
    { this->afterExecuteJob(worker, job); }

    inline void handleBeforeExecuteJob(const PoolWorkerKey&, std::shared_ptr<WorkerThread> worker, AbstractJob& job)
    { this->beforeExecuteJob(worker, job); }

    inline void handleOnStartupWorker(const PoolWorkerKey&, std::shared_ptr<WorkerThread> worker)
    {
        this->onStartupWorker(worker);

        ++this->num_alive_workers;
    }

    void handleOnShutdownWorker(const PoolWorkerKey&, std::shared_ptr<WorkerThread> worker);

    inline void handleOnTimeoutWorker(const PoolWorkerKey&, std::shared_ptr<WorkerThread> worker)
    { this->onTimeoutWorker(worker); }

    inline void handleOnSynchronizeWorker(const PoolWorkerKey&, std::shared_ptr<WorkerThread> worker)
    { this->onSynchronizeWorker(worker); }
};

} // end namespace detail

} // end namespace ride
