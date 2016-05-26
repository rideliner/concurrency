
#pragma once

#include <memory>
#include <thread>
#include <forward_list>

#include <ride/concurrency/job.hpp>
#include <ride/concurrency/containers/deque.hpp>

namespace ride {

namespace detail {

class WorkerThread;
class WorkerThreadFactory;

template <class Mutex_>
class Barrier;

} // end namespace detail

class ThreadPool
{
  public:
    typedef std::unique_ptr<detail::AbstractJob> PolymorphicJob;
    typedef ConcurrentDeque<PolymorphicJob> WorkContainer;
    typedef std::unique_ptr<detail::WorkerThread> PolymorphicWorker;
    typedef std::shared_ptr<detail::WorkerThreadFactory> PolymorphicWorkerFactory;
  private:
    typedef std::mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    typedef std::unique_ptr<Lock> LockPtr;
    typedef std::lock_guard<Mutex> LockGuard;
    typedef detail::Barrier<Mutex> Barrier;

    WorkContainer work;
    mutable Mutex thread_management;
    std::atomic_size_t num_pseudo_workers, num_alive_workers;
    std::shared_ptr<Barrier> join_barrier;
    std::forward_list<PolymorphicWorker> workers;

    inline void getJob(PolymorphicJob&& job)
    { this->work.popFront(std::move(job)); }

    void safeRemoveWorkersWhen(std::size_t to_remove, LockPtr lock, std::function<void(PolymorphicJob&&)> how_to_remove);

    inline void safeRemoveWorkersNow(std::size_t to_remove, LockPtr lock)
    {
        safeRemoveWorkersWhen(to_remove, std::move(lock), std::bind(static_cast<void(WorkContainer::*)(PolymorphicJob&&)>(&WorkContainer::pushFront), std::ref(this->work), std::placeholders::_1));
    }

    inline void safeRemoveWorkersLater(std::size_t to_remove, LockPtr lock)
    {
        safeRemoveWorkersWhen(to_remove, std::move(lock), std::bind(static_cast<void(WorkContainer::*)(PolymorphicJob&&)>(&WorkContainer::pushBack), std::ref(this->work), std::placeholders::_1));
    }

    bool unsafeIsCurrentThreadInPool() const;

    void safeJoin(bool remove_workers);

    inline void synchronizeWorkers(std::size_t num_workers, std::shared_ptr<Barrier> barrier)
    {
        for (std::size_t i = 0; i < num_workers; ++i)
            this->work.pushBack(this->createSyncPill(barrier));
    }

    std::size_t setupBarrier(std::shared_ptr<Barrier>& barrier) const;

    static inline PolymorphicJob createPoisonPill(std::shared_ptr<Barrier> barrier)
    {
        return static_cast<PolymorphicJob>(new detail::PoisonJob<Mutex>(barrier));
    }

    static inline PolymorphicJob createSyncPill(std::shared_ptr<Barrier> barrier)
    {
        return static_cast<PolymorphicJob>(new detail::SynchronizeJob<Mutex>(barrier));
    }

    inline void handleAfterExecuteJob(detail::WorkerThread& worker, detail::AbstractJob& job)
    { this->afterExecuteJob(worker, job); }

    inline void handleBeforeExecuteJob(detail::WorkerThread& worker, detail::AbstractJob& job)
    { this->beforeExecuteJob(worker, job); }

    inline void handleOnStartupWorker(detail::WorkerThread& worker)
    {
        this->onStartupWorker(worker);

        ++this->num_alive_workers;
    }

    void handleOnShutdownWorker(detail::WorkerThread& worker);

    inline void handleOnTimeoutWorker(detail::WorkerThread& worker)
    { this->onTimeoutWorker(worker); }

    inline void handleOnSynchronizeWorker(detail::WorkerThread& worker)
    { this->onSynchronizeWorker(worker); }

    inline virtual void afterExecuteJob(detail::WorkerThread& worker, detail::AbstractJob& job) { }
    inline virtual void beforeExecuteJob(detail::WorkerThread& worker, detail::AbstractJob& job) { }
    inline virtual void onStartupWorker(detail::WorkerThread& worker) { }
    inline virtual void onShutdownWorker(detail::WorkerThread& worker) { }
    inline virtual void onTimeoutWorker(detail::WorkerThread& worker) { }
    inline virtual void onSynchronizeWorker(detail::WorkerThread& worker) { }
  public:
    ThreadPool()
      : num_pseudo_workers(0)
      , num_alive_workers(0)
      , join_barrier(nullptr)
    { }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;
    virtual ~ThreadPool() = default;

    template <class Func_>
    inline static PolymorphicJob createJob(Func_ function)
    { return PolymorphicJob(new Job<typename Func_::result_type>(function)); }

    template <class Func_>
    inline void emplaceJob(Func_ function)
    { this->work.emplaceBack(new Job<typename Func_::result_type>(function)); }

    template <class Func_>
    inline void emplacePriorityJob(Func_ function)
    { this->work.emplaceFront(new Job<typename Func_::result_type>(function)); }

    template <class T_>
    inline void addJob(std::unique_ptr<Job<T_>>&& job_ptr)
    { this->work.pushBack(std::move(job_ptr)); }

    template <class T_>
    inline void addPriorityJob(std::unique_ptr<Job<T_>>&& job_ptr)
    { this->work.pushFront(std::move(job_ptr)); }

    inline void removeWorker()
    {
        LockPtr lock(new Lock(this->thread_management));
        this->safeRemoveWorkersNow(1, std::move(lock));
    }

    inline void removeWorkers(std::size_t to_remove)
    {
        LockPtr lock(new Lock(this->thread_management));
        this->safeRemoveWorkersNow(to_remove, std::move(lock));
    }

    inline void removeWorkerLater()
    {
        LockPtr lock(new Lock(this->thread_management));
        this->safeRemoveWorkersLater(1, std::move(lock));
    }

    inline void removeWorkersLater(std::size_t to_remove)
    {
        LockPtr lock(new Lock(this->thread_management));
        this->safeRemoveWorkersLater(to_remove, std::move(lock));
    }

    inline void removeAllWorkers()
    {
        LockPtr lock(new Lock(this->thread_management));
        this->safeRemoveWorkersNow(this->numWorkers(), std::move(lock));
    }

    inline void removeAllWorkersLater()
    {
        LockPtr lock(new Lock(this->thread_management));
        this->safeRemoveWorkersLater(this->numWorkers(), std::move(lock));
    }

    inline void addWorker(PolymorphicWorkerFactory factory)
    { this->addWorkers(1, factory); }

    void addWorkers(std::size_t to_create, PolymorphicWorkerFactory factory);

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

    bool isCurrentThreadInPool() const;

    friend class detail::WorkerThread;
};

} // end namespace ride

#include <ride/concurrency/detail/worker.hpp>
#include <ride/concurrency/detail/worker_factory.hpp>

