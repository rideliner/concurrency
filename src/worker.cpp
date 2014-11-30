
#include <ride/concurrency/detail/worker.hpp>

namespace ride { namespace detail {

void WorkerThread::run()
{
    this->handleOnStartup();

    std::unique_ptr<AbstractJob> job = nullptr;
    bool timedout;

    while (!this->is_finished)
    {
        timedout = this->getJob(std::move(job));

        if (timedout)
        {
            this->handleOnTimeout();
            continue;
        }

        if (!job)
            continue;

        if (job->isPoison())
        {
            this->handleOnShutdown();
            continue;
        }

        this->handleBeforeExecute(*job);
        job->operator()();
        this->handleAfterExecute(*job);
    }
}

} // end namespace detail

} // end namespace ride

