#include <iostream>
#include "clause.h"
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __abs(a) (((a) >= 0) ? (a) : (-a))

void Clause::set_state(unsigned int index, Clause::State state) {
    normal_mask.set_bit(index, state == State::Normal);
    not_mask.set_bit(index, state == State::Not);
}

Clause::Clause(int v1, int v2, int v3, unsigned int print_size) {
    nb_var = __max(__abs(v1), __max(__abs(v2), __abs(v3)));

    normal_mask = BitString(print_size, nb_var);
    not_mask = BitString(print_size, nb_var);

    if (v1 != 0) set_state(__abs(v1) - 1, v1 > 0 ? State::Normal : State::Not);
    if (v2 != 0) set_state(__abs(v2) - 1, v2 > 0 ? State::Normal : State::Not);
    if (v3 != 0) set_state(__abs(v3) - 1, v3 > 0 ? State::Normal : State::Not);
}

bool Clause::evaluate(const BitString& assignation) const {
    for (size_t i = 0; i < normal_mask.get_uint64_string().size(); i++)
    {
        std::uint64_t assign = assignation.get_uint64_string()[i];
        std::uint64_t add = normal_mask.get_uint64_string()[i];
        std::uint64_t remove = not_mask.get_uint64_string()[i];
        if (((assign & add) | (~assign & remove)) != 0ULL) return true;
    }
    return false;
    
    // return !((assignation & normal_mask) | (~assignation & not_mask)).is_null();
}

std::ostream& operator<<(std::ostream& c, const Clause& clause) {
    return std::cout << "mask+ : " << clause.normal_mask << "\nmask- : " << clause.not_mask << "\n";
}
