// Copyright (c) 2016 Nathan Currier

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ride/concurrency/pool.hpp>

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

        if (job->isPoison()) {
            this->handleOnShutdown();
            job->operator()(key);
            continue;
        } else if (job->isSync()) {
            this->handleOnSynchronize();
            job->operator()(key);
        } else {
            this->handleBeforeExecute(*job);
            job->operator()(key);
            this->handleAfterExecute(*job);
        }
    }
}

} // end namespace detail

} // end namespace ride

