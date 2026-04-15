#include "random_utils.h"

bool RandomUtils::get_bool(float proba, std::mt19937& rand) {
    std::uniform_real_distribution<float> dist(0, 1);
    return dist(rand) < proba;
}
unsigned int RandomUtils::get_index(unsigned int max, std::mt19937& rand) {
    if (max == 0) {
        std::cerr << "\033[1;31mERROR: max cannot be 0.\nWhile generating a random index\033[0m\n";
        exit(1);
    }
    return std::uniform_int_distribution<>(0, max - 1)(rand);
}
unsigned int RandomUtils::get_index(unsigned int max, std::vector<unsigned int> blacklist, std::mt19937& rand) {
    if (blacklist.size() >= max) {
        std::cerr << "\033[1;31mERROR: more value in the blacklist than in the range [0;"<< max <<"[.\nWhile generating a random index\033[0m\n";
        exit(1);
    }

    unsigned int i = get_index(max - blacklist.size(), rand);
    while (std::find(blacklist.begin(), blacklist.end(), i) != blacklist.end()) i++;
    return i;
}
unsigned int RandomUtils::get_index(unsigned int max, std::list<unsigned int> blacklist, std::mt19937& rand) {
    if (blacklist.size() >= max) {
        std::cerr << "\033[1;31mERROR: more value in the blacklist than in the range [0;"<< max <<"[.\nWhile generating a random index\033[0m\n";
        exit(1);
    }

    unsigned int i = get_index(max - blacklist.size(), rand);
    while (std::find(blacklist.begin(), blacklist.end(), i) != blacklist.end()) i++;
    return i;
}
unsigned int RandomUtils::get_index(unsigned int max, std::set<unsigned int> blacklist, std::mt19937& rand) {
    if (blacklist.size() >= max) {
        std::cerr << "\033[1;31mERROR: more value in the blacklist than in the range [0;"<< max <<"[.\nWhile generating a random index\033[0m\n";
        exit(1);
    }

    unsigned int i = get_index(max - blacklist.size(), rand);
    for (unsigned int v : blacklist) if (i == v) i++;
    return i;
}