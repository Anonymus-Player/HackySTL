#pragma once

#include "Thread.hpp"
#include "MPMCQueue.hpp"
#include "Functional.hpp"

namespace hsd
{
    struct Job;

    using job_fn = function<void(Job)>;

    // A Job. Stores a funciton and a pointer to data to pass as args
    struct Job
    {
        job_fn task = job_fn();
        void** data = nullptr;

        Job() noexcept = default;
	    Job(Job&&) noexcept = default;
	    Job(Job const&) noexcept = default;
	    Job& operator=(Job&&) noexcept = default;
	    Job& operator=(Job const&) noexcept = default;
	    ~Job() noexcept = default;
    };

/// @TODO: Change from using a void** to pass args (which is super bad and unsafe), to this JobData struct
/*
    // A packed list of arguments for a function in a job
    template<u8 numargs, typename T>
    struct JobData
    {
        
    };
*/

    enum class PRIO
    {
        HIGH = 0,
        NORM,
        LOW,

        NUM_PRIORITIES = 3
    };

    namespace priv
    {
        class JobSystem
        {
        private:
            counter _counter = {0};
            atomic_bool _running = {false};

            MPMCQueue<Job> _high_priority;
            MPMCQueue<Job> _normal_priority;
            MPMCQueue<Job> _low_priority;

            vector<thread> _threads;

            void thread_function()
            {
                Job _job;
                while(_running.load())
                {
                    if(_high_priority.try_pop(_job))
                    {
                        _job.task(_job);
                    }
                    else if(_normal_priority.try_pop(_job))
                    {
                        _job.task(_job);
                    }
                    else if(_low_priority.try_pop(_job))
                    {
                        _job.task(_job);
                    }
                }
            }

        public:
            JobSystem()
                : _high_priority(512), 
                _normal_priority(512), 
                _low_priority(512)
            {
                _running.store(true);
                auto _numcores = thread::hardware_concurrency(); 

                for(auto _index = 0; _index < _numcores; _index++)
                {
                    _threads.emplace_back(bind(&JobSystem::thread_function, this));
                }
            }

            ~JobSystem()
            {
                _running.store(false);

                for(auto& _thread : _threads)
                {
                    _thread.join();
                }
            }

            //Returns the id of the caller from the threadpool index [0, max), and returns -1 if the caller isn't in the threadpool
            usize get_current_thread() const
            {
                auto _this_thread_id = this_thread::get_id();
                for(usize _index = 0; _index < _threads.size(); _index++)
                {
                    if(_this_thread_id == _threads[_index].get_id())
                    {
                        return _index;
                    }
                }

                return -1;
            }

            // Returns a Job which may be scheduled.
            static Job create_job(job_fn jobfunction, void** data = nullptr)
            {
                return Job{
                    .task = jobfunction,
                    .data = data
                };
            }

            // Schedules a job to be added to the queue. Optionally accepts a priority for the job
            void schedule_job(Job& job, PRIO priority = PRIO::NORM)
            {
                switch(priority)
                {
                    case PRIO::HIGH:
                        _high_priority.emplace(forward<Job>(job));
                        _counter++;
                        return;
                    case PRIO::NORM:
                        _normal_priority.emplace(forward<Job>(job));
                        _counter++;
                        return;
                    case PRIO::LOW:
                        _low_priority.emplace(forward<Job>(job));
                        _counter++;
                        return;
                    default:
                        return;
                }
            }
            // Wait until the number of remaining tasks
            // is equal to or below the `target`, the
            // second argument will use the caller thread
            // to execute jobs as well if set to `true`
            void wait(ulong target = 0, bool use_current_thread = true)
            {
                while(_counter.load() > target)
                {
                    if(use_current_thread)
                    {
                        Job _job;
                        if(_high_priority.try_pop(_job))
                        {
                            _job.task(_job);
                            _counter--;
                        }
                        else if(_normal_priority.try_pop(_job))
                        {
                            _job.task(_job);
                            _counter--;
                        }
                        else if(_low_priority.try_pop(_job))
                        {
                            _job.task(_job);
                            _counter--;
                        }
                    }
                }
            }
        
        };
    }

    // Global Job System. It is not recommended to make a new one, as the threads last for the lifetime of the program (global)
    static inline priv::JobSystem job_system;
    
}
