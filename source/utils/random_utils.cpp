#include "random_utils.h"

bool RandomUtils::get_bool(float proba) {
    return ((float)std::rand() / (float)RAND_MAX) < proba;
}
unsigned int RandomUtils::get_index(unsigned int max) {
    if (max == 0) {
        std::cerr << "\033[1;31mERROR: max cannot be 0.\nWhile generating a random index\033[0m\n";
        exit(1);
    }
    return (unsigned int)std::rand() % max;
}
unsigned int RandomUtils::get_index(unsigned int max, std::vector<unsigned int> blacklist) {
    if (blacklist.size() >= max) {
        std::cerr << "\033[1;31mERROR: more value in the blacklist than in the range [0;"<< max <<"[.\nWhile generating a random index\033[0m\n";
        exit(1);
    }

    unsigned int i = get_index(max - blacklist.size());
    while (std::find(blacklist.begin(), blacklist.end(), i) != blacklist.end()) i++;
    return i;
}
unsigned int RandomUtils::get_index(unsigned int max, std::list<unsigned int> blacklist) {
    if (blacklist.size() >= max) {
        std::cerr << "\033[1;31mERROR: more value in the blacklist than in the range [0;"<< max <<"[.\nWhile generating a random index\033[0m\n";
        exit(1);
    }

    unsigned int i = get_index(max - blacklist.size());
    while (std::find(blacklist.begin(), blacklist.end(), i) != blacklist.end()) i++;
    return i;
}
unsigned int RandomUtils::get_index(unsigned int max, std::set<unsigned int> blacklist) {
    if (blacklist.size() >= max) {
        std::cerr << "\033[1;31mERROR: more value in the blacklist than in the range [0;"<< max <<"[.\nWhile generating a random index\033[0m\n";
        exit(1);
    }

    unsigned int i = get_index(max - blacklist.size());
    for (unsigned int v : blacklist) if (i == v) i++;
    return i;
}
std::uint64_t RandomUtils::get_uint64()
{
    return rand() ^
        ((std::uint64_t)rand() << 15) ^
        ((std::uint64_t)rand() << 30) ^
        ((std::uint64_t)rand() << 45) ^
        ((std::uint64_t)rand() << 60);
}