///////////////////////////////////////////////////////////////////////////////////////////////
//                                   BSD 2-Clause License                                    // 
///////////////////////////////////////////////////////////////////////////////////////////////
//                             Copyright (c) 2025, Sandia National Laboratories              // 
//                                                                                           // 
// Redistribution and use in source and binary forms, with or without modification, are      // 
// permitted provided that the following conditions are met:                                 // 
//                                                                                           // 
// 1. Redistributions of source code must retain the above copyright notice, this            // 
//    list of conditions and the following disclaimer.                                       // 
//                                                                                           // 
// 2. Redistributions in binary form must reproduce the above copyright notice,              // 
//    this list of conditions and the following disclaimer in the documentation              // 
//    and/or other materials provided with the distribution.                                 // 
//                                                                                           // 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"               // 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE                 // 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            // 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE              // 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL                // 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR                // 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER                // 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,             // 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE             // 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      //
///////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
    : _num_threads(num_threads),
    _threads(),
    _threads_busy(),
    _should_terminate(false),
    _queue_mutex(),
    _mutex_condition(),
    _active_threads(0)
{}

ThreadPool::~ThreadPool()
{
    if (_active_threads > 0)
    {
        stop();
    }
}

void ThreadPool::start()
{
    _threads = new std::thread[_num_threads];
    _threads_busy = new bool[_num_threads]();
    for (size_t i = 0; i < _num_threads; i++) {
        _threads[i] = std::thread(&ThreadPool::thread_loop, this, i);
    }
    _active_threads = _num_threads;
}

void ThreadPool::thread_loop(size_t thread_index) 
{
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _threads_busy[thread_index] = false;
            _mutex_condition.wait(lock, [this] {
                return !_jobs.empty() || _should_terminate;
                });
            if (_should_terminate) {
                return;
            }
            _threads_busy[thread_index] = true;

            job = _jobs.front();
            _jobs.pop();
        }
        job();
    }
}

void ThreadPool::queue_job(const std::function<void()>& job) 
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _jobs.push(job);
    }
    _mutex_condition.notify_one();
}

bool ThreadPool::busy() 
{
    std::unique_lock<std::mutex> lock(_queue_mutex);
    if (!_jobs.empty())
    {
        return true;
    }

    for (int i = 0; i < _num_threads; i++)
    {
        if (_threads_busy[i])
        {
            return true;
        }
    }

    return false;
}

void ThreadPool::stop() 
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _should_terminate = true;
    }
    _mutex_condition.notify_all();
    for(size_t i = 0; i < _num_threads; i++)
    {
        _threads[i].join();
    }
    delete[] _threads;
    delete[] _threads_busy;
    _active_threads = 0;
}
