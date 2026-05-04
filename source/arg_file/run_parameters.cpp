#include "run_parameters.h"
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

bool RunParameters::is_opening_label(const std::string& keyword) {
    return keyword.length() > 3 && keyword[0] == '<' && keyword[keyword.length() - 1] == '>' && is_valid_label(keyword.substr(1, keyword.length() - 2));
}
bool RunParameters::is_closing_label(const std::string& keyword) {
    return keyword.length() > 4 && keyword[0] == '<' && keyword[1] == '/' && keyword[keyword.length() - 1] == '>' && is_valid_label(keyword.substr(2, keyword.length() - 3));
}
bool RunParameters::is_valid_label(const std::string& label) {
    return label == "infos" || label == "algo" || label == "instance";
}

RunParameters::RunParameters(const std::string& path): Parameters(path), instances(), algos() {
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open data_file at \"" << path << "\" !\033[0m\n";
        exit(1);
    }

    std::string keyword;
    std::string line;
    int l_count = 0;
    while (std::getline(file, line))
    {
        l_count++;
        if (is_blank(line)) continue;
        
        std::istringstream stream(line);
        stream >> keyword;

        if (!is_opening_label(keyword)) {
            print_error(line, l_count, "Invalid opening label ! Expected <infos>, <algo> or <instance>. Got \"" + keyword + "\"");
        }
        std::string label = keyword.substr(1, keyword.length() - 2);

        unsigned int labeled_section_line = l_count + 1;
        std::string labeled_section = "";
        bool closed = false;
        while (std::getline(file, line))
        {
            l_count++;
            if (is_blank(line)) {
                labeled_section += line + "\n";
                continue;
            }
            
            std::istringstream stream(line);
            stream >> keyword;

            if (is_opening_label(keyword)) print_error(line, l_count, "Can't open the label " + keyword + " because another label <" + label + "> is open and has not been closed !");
            if (!is_closing_label(keyword)) {
                labeled_section += line + "\n";
                continue;
            }
            std::string closed_label = keyword.substr(2, keyword.length() - 3);
            if (label != closed_label) {
                print_error(line, l_count, "Closed label does not corespond to the opened one ! Expected " + label + ". Got \"" + closed_label + "\"");
            }

            closed = true;
            break;
        }

        if (!closed) print_error(line, l_count, "Opened label " + label + " is never closed !");

        // std::cout << "--- parsing ----------\n" << labeled_section << "---for label " << label << " ----------" << std::endl;

        std::istringstream section(labeled_section);
        if (label == "infos") this->parse(section, labeled_section_line);
        else if (label == "algo") parse_with_iterators(section, labeled_section_line, path, algos);
        else if (label == "instance") parse_with_iterators(section, labeled_section_line, path, instances);
    }
}

void RunParameters::set_seed(unsigned int seed) { float_vars["seed"] = seed; }
void RunParameters::set_default_seed(unsigned int seed) { if (!contains_bool("seed")) float_vars["seed"] = seed; }
unsigned int RunParameters::get_seed() const { return get_int("seed"); }

const std::vector<Parameters>& RunParameters::get_algos() const { return algos; }
const std::vector<Parameters>& RunParameters::get_instances() const { return instances; }

std::ostream& RunParameters::cout(std::ostream& c) const {
    Parameters::cout(c << "============ Infos ==============");

    c << "============ Instances ==============" << std::endl;
    for (const Parameters& p : this->instances) c << p << std::endl;

    c << "============ Algos ==============" << std::endl;
    for (const Parameters& p : this->algos) c << p << std::endl;

    return c;
}
