
#pragma once

#include <ride/concurrency/pool.hpp>

namespace ride {

namespace detail {

class WorkerThread
{
  protected:
    ride::ThreadPool& pool;
    bool is_finished;
    std::thread thread;
  private:
    void run();

    inline void handleBeforeExecute(AbstractJob& job)
    {
        this->beforeExecute(job);
        this->pool.handleBeforeExecuteJob(*this, job);
    }

    inline void handleAfterExecute(AbstractJob& job)
    {
        this->afterExecute(job);
        this->pool.handleAfterExecuteJob(*this, job);
    }

    inline void handleOnStartup()
    {
        this->onStartup();
        this->pool.handleOnStartupWorker(*this);
    }

    inline void handleOnShutdown()
    {
        this->is_finished = true;
        this->thread.detach();

        this->onShutdown();
        this->pool.handleOnShutdownWorker(*this);
    }

    inline void handleOnTimeout()
    {
        this->onTimeout();
        this->pool.handleOnTimeoutWorker(*this);
    }

    inline void handleOnSynchronize()
    {
        this->onSynchronize();
        this->pool.handleOnSynchronizeWorker(*this);
    }

    inline virtual void beforeExecute(AbstractJob& job) { }
    inline virtual void afterExecute(AbstractJob& job) { }
    inline virtual void onStartup() { }
    inline virtual void onShutdown() { }
    inline virtual void onTimeout() { }
    inline virtual void onSynchronize() { }

    inline virtual bool getJob(std::unique_ptr<AbstractJob>&& job)
    { this->pool.getJob(std::move(job)); return false; }
  public:
    WorkerThread() = delete;
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator = (const WorkerThread&) = delete;
    virtual ~WorkerThread() { }

    WorkerThread(ThreadPool& owner)
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

