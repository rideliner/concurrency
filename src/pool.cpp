
#include <ride/concurrency/pool.hpp>
#include <ride/concurrency/detail/worker.hpp>
#include <ride/concurrency/detail/worker_factory.hpp>

#include <algorithm>

namespace ride {

void ThreadPool::addWorkers(std::size_t to_create, PolymorphicWorkerFactory factory)
{
    this->num_pseudo_workers += to_create;

    for (std::size_t i = 0; i < to_create; ++i)
        workers.push_front(factory->create(*this));
}

void ThreadPool::safeRemoveWorkersWhen(std::size_t to_remove, LockPtr lock, std::function<void(PolymorphicJob&&)> how_to_remove)
{
    to_remove = std::min(to_remove, this->numWorkers());

    this->num_pseudo_workers -= to_remove;

    if (lock)
        lock->unlock();

    for (std::size_t i = 0; i < to_remove; ++i)
        how_to_remove(this->createPoisonPill());
}

void ThreadPool::collect(std::function<void(std::size_t)> action)
{
    Lock lock(this->thread_management);

    // don't allow collection from a worker thread
    if (this->unsafeIsCurrentThreadInPool())
    {
        lock.unlock();
        return;
    }

    // if a barrier already exists, just wait for that barrier to open
    if (this->barrier)
    {
        this->barrier->wait(lock);
        lock.unlock();
        return;
    }

    std::size_t num_workers = this->numWorkers();

    if (num_workers == 0)
    { // don't setup a barrier if there are no workers
        lock.unlock();
        return;
    }

    this->barrier.reset(new detail::Barrier(num_workers));

    action(num_workers);

    // wait for the barrier to open, then destroy it
    this->barrier->wait(lock);
    this->barrier.reset();

    lock.unlock();
}

void ThreadPool::join()
{
    collect(std::bind(&ThreadPool::safeRemoveWorkersLater, std::ref(*this), std::placeholders::_1, nullptr));
}

void ThreadPool::synchronize()
{
    collect([&](std::size_t to_sync) {
        for (std::size_t i = 0; i < to_sync; ++i)
            this->work.pushBack(this->createSyncPill());
    });
}

bool ThreadPool::unsafeIsCurrentThreadInPool() const
{
    std::thread::id id = std::this_thread::get_id();

    return std::any_of(this->workers.begin(), this->workers.end(), [&id](const PolymorphicWorker& x) -> bool { return x->getId() == id; });
}

bool ThreadPool::isCurrentThreadInPool() const
{
    LockGuard lock(this->thread_management);

    return unsafeIsCurrentThreadInPool();
}

void ThreadPool::handleOnShutdownWorker(detail::WorkerThread& worker)
{
    this->onShutdownWorker(worker);

    std::thread::id id = worker.getId();
    Lock lock(this->thread_management);

    this->workers.remove_if([&id](const PolymorphicWorker& x) -> bool { return x->getId() == id; });
    if (this->barrier)
        this->barrier->unblock();

    lock.unlock();

    --this->num_alive_workers;
}

void ThreadPool::handleOnSynchronizeWorker(detail::WorkerThread& worker)
{
    this->onSynchronizeWorker(worker);

    Lock lock(this->thread_management);

    if (this->barrier)
        this->barrier->unblockAndWait(lock);

    lock.unlock();
}

} // end namespace ride

