#pragma once
#include <memory>
#include "run_parameters.h"
#include "../problem/nk.h"

class Job
{
protected:
    RunParameters* parameters;
    unsigned int algo_index, instance_index;
public:
    Job(RunParameters* parameters, unsigned int algo_index, unsigned int instance_index);
    Job() = default;

    const Parameters& get_run_infos() const;
    const Parameters& get_algo_parameters() const;
    const Parameters& get_instance_parameters() const;

    /// @return the path on wich the rundata will be (or is) saved
    std::string get_output_file_path() const;

    /// @param debug whether debuging informations should bu put on the cout stream
    /// @return the path on wich the rundata will be (or is) saved and a stream to fill if the data need to be saved at the location
    std::pair<std::string, std::ostringstream*> get_output_pair(bool debug = true) const;

    /// @brief executs the job
    /// @param debug whether the algo should debug informations on the cout stream
    /// @return the path to the saved data (or "" if no data was saved)
    std::string execute_job(bool debug = true) const;
};