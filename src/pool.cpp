// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ride/concurrency/pool.hpp>
#include <ride/concurrency/detail/barrier.hpp>

namespace ride {

std::pair<std::thread::id, ThreadPool::PolymorphicWorker> ThreadPool::createWorker(PolymorphicWorkerFactory factory)
{
    PolymorphicWorker worker = factory->create(creatorKey, *this);
    return std::make_pair(worker->getId(), std::move(worker));
}

void ThreadPool::safeAddWorkers(std::size_t to_create, PolymorphicWorkerFactory factory, LockPtr lock)
{
    this->num_pseudo_workers += to_create;

    for (std::size_t i = 0; i < to_create; ++i)
        workers.insert(createWorker(factory));

    if (lock)
        lock->unlock();
}

std::size_t ThreadPool::safeRemoveWorkers(std::size_t to_remove, LockPtr lock)
{
    to_remove = std::min(to_remove, this->numWorkers());

    this->num_pseudo_workers -= to_remove;

    if (lock)
        lock->unlock();

    return to_remove;
}

std::size_t ThreadPool::setupBarrier(std::shared_ptr<detail::Barrier>& barrier) const
{
    std::size_t num_workers = this->numWorkers();

    if (num_workers > 0)
        barrier.reset(new detail::Barrier(num_workers));

    return num_workers;
}

void ThreadPool::safeJoin(bool remove_workers)
{
    Lock lock(this->thread_management);

    // don't allow a join from a worker thread
    if (this->unsafeIsCurrentThreadInPool())
    {
        lock.unlock();
        return;
    }

    // if a barrier already exists, just wait for that barrier to open
    // copy the shared_ptr so it doesn't get reset while waiting on it
    if (std::shared_ptr<detail::Barrier> barrier = this->join_barrier)
    {
        lock.unlock();
        barrier->wait();
        return;
    }

    if (std::size_t num_workers = setupBarrier(this->join_barrier))
    { // don't setup a barrier if there are no workers
        if (remove_workers)
            safeRemoveWorkersLater(num_workers, nullptr);
        else
            synchronizeWorkers(num_workers, this->join_barrier);

        lock.unlock();

        // wait for the barrier to open, then destroy it
        this->join_barrier->wait();
        this->join_barrier.reset();
    }
    else
        lock.unlock();
}

void ThreadPool::sync()
{
    Lock lock(this->thread_management);
    std::shared_ptr<detail::Barrier> barrier;

    synchronizeWorkers(setupBarrier(barrier), barrier);

    lock.unlock();
}

void ThreadPool::handleOnShutdownWorker(const detail::PoolWorkerKey&, detail::WorkerThread& worker)
{
    this->onShutdownWorker(worker);

    Lock lock(this->thread_management);

    this->workers.erase(worker.getId());

    lock.unlock();

    --this->num_alive_workers;
}

} // end namespace ride

