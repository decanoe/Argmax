#include "job_queue.h"

void JobQueue::worker_thread(unsigned int thread_index) {
    double last_job_execution_time = -1;
    while (true)
    {
        // gather job
        Job job;
        {
            std::lock_guard lock(jobs_mutex);
            if (jobs.empty()) break;
            job = jobs.front();
            jobs.pop_front();
        }
        
        // shows this data as running (progress bar)
        {
            std::string job_path = job.get_output_file_path();

            std::lock_guard lock(running_files_mutex);
            running_files[thread_index] = (job_path == "") ? "unlabled run" : job_path;
            if (last_job_execution_time >= 0) {
                runned_file_count++;
                runned_durations_sum += last_job_execution_time;
            }
        }
        condition.notify_all();

        // execute file
        job.execute_job(false);
        last_job_execution_time = job.get_execution_duration();
    }

    // shows this threas as out of job (progress bar)
    {
        std::lock_guard lock(running_files_mutex);
        running_files[thread_index] = "finished";
        runned_file_count++;
        runned_durations_sum += last_job_execution_time;
    }
    condition.notify_all();
}

float inv_lerp(float from, float to, float value){
    return (value - from) / (to - from);
}
float lerp(float from, float to, float value){
    return ((1 - value) * from) + (value * to);
}
/// @brief prints a progress
/// @param value the fill value from 0 to 1
/// @param char_count the size of the progress bar
void cout_bar(float value, int char_count = 100) {
    std::string chars[] {"█", "▉", "▊", "▋", "▌", "▍", "▎", "▏"};
    std::cout << "[";
    for (int i = 0; i < char_count; i++)
    {
        if (i < value * char_count) std::cout << "█";
        else if (i + 1 > value * char_count) std::cout << " ";
        else {
            float v = inv_lerp(i, i+1, value * char_count);
            std::cout << chars[(int)lerp(0, 8, v)];
        }
    }
    std::cout << "] (" << ((int)(value * 100)) << "%)";
}
std::string format_duration(double duration) {
    int nb_s = duration;
    duration -= nb_s;
    int nb_m = nb_s / 60;
    nb_s -= nb_m * 60;
    int nb_h = nb_m / 60;
    nb_m -= nb_h * 60;

    int nb_ms = duration * 100;

    std::string result = "";
    if (nb_h != 0) result += std::to_string(nb_h) + "h ";
    if (nb_m != 0) result += std::to_string(nb_m) + "m ";
    if (nb_s != 0) result += std::to_string(nb_s) + "s ";
    return result + std::to_string(nb_ms) + "ms";
}
void JobQueue::progress_bar_thread(unsigned int total_job_count) {
    unsigned int count = -1U;
    unsigned int go_up_n_lines = 0;
    std::cout << std::endl;
    while (count != total_job_count)
    {
        std::unique_lock<std::mutex> lock(running_files_mutex);
        while (count == runned_file_count) condition.wait(lock);
        count = std::min(runned_file_count, total_job_count);

        std::cout << "\033[" + std::to_string(go_up_n_lines) + "A"; // go up a specific number of lines
        std::cout << "\r\033[2K";
        cout_bar(total_job_count == 0 ? 1 : (float)count / total_job_count);
        std::cout << "\t" << count << " / " << total_job_count << std::flush;
        go_up_n_lines = 0;

        if (count != 0) {
            double avg_duration = (runned_durations_sum / count);
            std::cout << "\n\r\033[2Kavg duration: " << format_duration(avg_duration) << std::flush;
            std::cout << "\t|\t" << format_duration(avg_duration * (total_job_count - count) / running_files.size()) << " remaining" << std::flush;
            go_up_n_lines++;
        }
        
        for (unsigned int i = 0; i < running_files.size(); i++)
            std::cout << "\n\r\033[2K\tthread " << i << ": " << running_files[i];
        go_up_n_lines += running_files.size();
        
        lock.unlock();
    }
}

JobQueue::JobQueue(RunParameters& run_parameters): jobs() {
    for (unsigned int algo = 0; algo < run_parameters.get_algos().size(); algo++)
    for (unsigned int instance = 0; instance < run_parameters.get_instances().size(); instance++)
    {
        Job job = Job(&run_parameters, algo, instance);
        auto output_pair = job.get_output_pair(false);
        if (output_pair.first == "" || output_pair.second != nullptr) this->jobs.push_back(job);
    }
}

void JobQueue::run_unthreaded() {
    unsigned int total_job_count = get_job_count();
    unsigned int count = 0;
    std::cout << std::endl;
    while (true)
    {
        std::cout << "\033[A"; // go up a 1 line
        std::cout << "\r\033[2K";
        cout_bar((float)count / total_job_count);
        std::cout << "\t" << count << " / " << total_job_count << std::flush;

        // get job
        Job job;
        {
            std::lock_guard lock(jobs_mutex);
            if (jobs.empty()) break;
            job = jobs.front();
            jobs.pop_front();
        }
        job.execute_job();

        count = total_job_count - get_job_count();
    }
}
void JobQueue::launch(unsigned int nb_threads) {
    if (nb_threads <= 1) return run_unthreaded();

    this->runned_file_count = 0;
    this->runned_durations_sum = 0;
    this->running_files = std::vector<std::string>((nb_threads == -1U) ? std::thread::hardware_concurrency() : nb_threads, "waiting for job");
    unsigned int total_job_count = get_job_count();

    // launch threads
    this->threads = std::list<std::thread>();
    for (unsigned int i = 0; i < running_files.size(); i++)
    {
        this->threads.push_back(std::thread(&JobQueue::worker_thread, this, i));
    }
    
    // progress bar
    this->threads.push_back(std::thread(&JobQueue::progress_bar_thread, this, total_job_count));
}
void JobQueue::join() {
    while (this->threads.size() != 0)
    {
        threads.front().join();
        threads.pop_front();
    }
}

unsigned int JobQueue::get_job_count() {
    std::lock_guard lock(jobs_mutex);
    return jobs.size();
}
