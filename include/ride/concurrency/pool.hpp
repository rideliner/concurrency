
#pragma once

#include <memory>
#include <thread>
#include <forward_list>

#include <ride/concurrency/job.hpp>
#include <ride/concurrency/containers/deque.hpp>
#include <ride/concurrency/detail/barrier.hpp>

namespace ride {

namespace detail {

class WorkerThread;
class WorkerThreadFactory;

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

    WorkContainer work;
    mutable Mutex thread_management;
    std::atomic_size_t num_pseudo_workers, num_alive_workers;
    std::unique_ptr<detail::Barrier> barrier;
    std::forward_list<PolymorphicWorker> workers;

    inline PolymorphicJob getJob()
    {
        PolymorphicJob job;
        this->getJob(std::move(job));
        return job;
    }

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

    void collect(std::function<void(std::size_t)> action);

    static inline PolymorphicJob createPoisonPill()
    {
        return static_cast<PolymorphicJob>(new detail::PoisonJob());
    }

    static inline PolymorphicJob createSyncPill()
    {
        return static_cast<PolymorphicJob>(new detail::SynchronizeJob());
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

    void handleOnSynchronizeWorker(detail::WorkerThread& worker);

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
      , barrier(nullptr)
    { }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator = (const ThreadPool&) = delete;
    virtual ~ThreadPool() = default;

    /*inline virtual PolymorphicWorkerFactory getDefaultWorkerFactory() const
    {
        static const PolymorphicWorkerFactory DefaultFactory(new detail::SimpleWorkerThreadFactory<detail::WorkerThread>());

        return DefaultFactory;
    }*/

    template <class T_, class... Args_>
    inline static PolymorphicJob createJob(Args_... args)
    { return PolymorphicJob(new Job<T_>(std::forward(args)...)); }

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

    void join();
    void synchronize();

    bool isCurrentThreadInPool() const;

    friend class detail::WorkerThread;
};

} // end namespace ride

#include <ride/concurrency/detail/worker.hpp>
#include <ride/concurrency/detail/worker_factory.hpp>

