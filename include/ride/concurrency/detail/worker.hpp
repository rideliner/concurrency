// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <ride/concurrency/pool.hpp>

namespace ride {

namespace detail {

class WorkerThread
{
    ride::detail::PoolWorkerKey key;
  protected:
    ride::ThreadPool& pool;
    bool is_finished;
    std::thread thread;
  private:
    void run();

    inline void handleBeforeExecute(AbstractJob& job)
    {
        this->beforeExecute(job);
        this->pool.handleBeforeExecuteJob(key, *this, job);
    }

    inline void handleAfterExecute(AbstractJob& job)
    {
        this->afterExecute(job);
        this->pool.handleAfterExecuteJob(key, *this, job);
    }

    inline void handleOnStartup()
    {
        this->onStartup();
        this->pool.handleOnStartupWorker(key, *this);
    }

    inline void handleOnShutdown()
    {
        this->is_finished = true;
        this->thread.detach();

        this->onShutdown();
        this->pool.handleOnShutdownWorker(key, *this);
    }

    inline void handleOnTimeout()
    {
        this->onTimeout();
        this->pool.handleOnTimeoutWorker(key, *this);
    }

    inline void handleOnSynchronize()
    {
        this->onSynchronize();
        this->pool.handleOnSynchronizeWorker(key, *this);
    }

    inline virtual void beforeExecute(AbstractJob& job) { }
    inline virtual void afterExecute(AbstractJob& job) { }
    inline virtual void onStartup() { }
    inline virtual void onShutdown() { }
    inline virtual void onTimeout() { }
    inline virtual void onSynchronize() { }

    // overload this method to call tryGetJobFromPool if getting a job could timeout
    inline virtual bool getJob(std::unique_ptr<AbstractJob>&& job)
    { return this->getJobFromPool(std::move(job)); }
  protected:
    inline bool getJobFromPool(std::unique_ptr<AbstractJob>&& job)
    { this->pool.getJob(key, std::move(job)); return false; }

    template <class Timeout_>
    inline bool tryGetJobFromPool(std::unique_ptr<AbstractJob>&& job, Timeout_&& timeout)
    { return this->pool.getJob(key, std::move(job), std::forward<Timeout_>(timeout)); }
  public:
    WorkerThread() = delete;
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator = (const WorkerThread&) = delete;
    virtual ~WorkerThread() = default;

    WorkerThread(const CreateWorkerKey&, ThreadPool& owner)
      : pool(owner)
      , is_finished(false)
      , thread(std::bind(&WorkerThread::run, std::ref(*this)))
    { }

    inline bool isCurrentThread() const
    { return this->getId() == std::this_thread::get_id(); }

    inline std::thread::id getId() const
    { return this->thread.get_id(); }
};

} // end namespace detail

} // end namespace ride

