#pragma once
#include "job.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class JobQueue
{
private:
    std::list<Job> jobs;
    std::mutex jobs_mutex;
    
    std::vector<std::string> running_files;
    unsigned int runned_file_count;
    std::mutex running_files_mutex;
    std::condition_variable condition;

    std::list<std::thread> threads;
    void worker_thread(unsigned int thread_index);
    void progress_bar_thread(unsigned int total_job_count);
    
    void run_unthreaded();
public:
    JobQueue(RunParameters& run_parameters);

    void launch(unsigned int nb_threads = -1U);
    void join();

    unsigned int get_job_count();
};
