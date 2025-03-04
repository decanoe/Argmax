#include <iostream>
#include "clause.h"
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __abs(a) (((a) >= 0) ? (a) : (-a))

Clause::Clause(int v1, int v2, int v3): normal_mask(0), not_mask(0) {
    nb_var = __max(__abs(v1), __max(__abs(v2), __abs(v3)));

    normal_mask = std::vector<std::uint64_t>(nb_var >> 6);
    not_mask = std::vector<std::uint64_t>(nb_var >> 6);

    if (v1 != 0) normal_mask[std::abs(v1) >> 6] |= 1 << (std::abs(v1) % (1 << 6));
    if (v2 != 0) normal_mask[std::abs(v2) >> 6] |= 1 << (std::abs(v2) % (1 << 6));
    if (v3 != 0) normal_mask[std::abs(v3) >> 6] |= 1 << (std::abs(v3) % (1 << 6));
}

bool Clause::evaluate(const std::vector<std::uint64_t>& assignation) const {
    for (int i = 0; i < nb_var >> 6; i++)
        if ((assignation[i] & normal_mask[i]) | (!assignation[i] & not_mask[i]) != 0) return true;
    
    return false;
}