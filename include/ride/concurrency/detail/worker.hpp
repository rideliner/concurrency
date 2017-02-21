// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/detail/pool.hpp>

namespace ride { namespace detail {

class WorkerThread
{
    PoolWorkerKey key;
  protected:
    std::shared_ptr<ThreadPool> pool;
    bool is_finished;
    std::unique_ptr<std::thread> thread;
  private:
    void run();

    inline void handleBeforeExecute()
    {
        this->beforeExecute();
        this->pool->handleBeforeExecuteJob(key);
    }

    inline void handleAfterExecute()
    {
        this->afterExecute();
        this->pool->handleAfterExecuteJob(key);
    }

    inline void handleOnStartup()
    {
        this->onStartup();
        this->pool->handleOnStartupWorker(key);
    }

    inline void handleOnShutdown()
    {
        this->is_finished = true;
        this->thread->detach();

        this->onShutdown();
        this->pool->handleOnShutdownWorker(key);
    }

    inline void handleOnTimeout()
    {
        this->onTimeout();
        this->pool->handleOnTimeoutWorker(key);
    }

    inline void handleOnSynchronize()
    {
        this->onSynchronize();
        this->pool->handleOnSynchronizeWorker(key);
    }

    inline virtual void beforeExecute() { }
    inline virtual void afterExecute() { }
    inline virtual void onStartup() { }
    inline virtual void onShutdown() { }
    inline virtual void onTimeout() { }
    inline virtual void onSynchronize() { }

    // overload this method to call tryGetJobFromPool if getting a job could timeout
    inline virtual bool getJob(std::unique_ptr<AbstractJob>&& job)
    { return this->getJobFromPool(std::move(job)); }
  protected:
    inline bool getJobFromPool(std::unique_ptr<AbstractJob>&& job)
    { this->pool->getJob(key, std::move(job)); return false; }

    template <class Timeout_>
    inline bool tryGetJobFromPool(std::unique_ptr<AbstractJob>&& job, Timeout_&& timeout)
    { return this->pool->tryGetJob(key, std::move(job), std::forward<Timeout_>(timeout)); }
  public:
    WorkerThread() = delete;
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator = (const WorkerThread&) = delete;

    WorkerThread(std::shared_ptr<ThreadPool> owner)
      : pool(owner)
      , is_finished(false)
      , thread(nullptr)
    { }

    inline void start(const StartWorkerKey&)
    {
        this->thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&WorkerThread::run, std::ref(*this))));
    }

    inline bool isCurrentThread() const
    { return this->getId() == std::this_thread::get_id(); }

    inline std::thread::id getId() const
    { return this->thread->get_id(); }
};

} // end namespace detail

} // end namespace ride

