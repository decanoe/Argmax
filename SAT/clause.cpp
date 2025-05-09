#include <iostream>
#include "clause.h"
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __abs(a) (((a) >= 0) ? (a) : (-a))

Clause::Clause(int v1, int v2, int v3) {
    var1 = __abs(v1) - 1;
    var2 = __abs(v2) - 1;
    var3 = __abs(v3) - 1;

    state1 = v1 > 0;
    state2 = v2 > 0;
    state3 = v3 > 0;
}

bool Clause::evaluate(const std::vector<bool>& assignation) const { return assignation[var1] == state1 || assignation[var2] == state2 || assignation[var3] == state3; }

std::ostream& operator<<(std::ostream& c, const Clause& clause) {
    return c << "(" <<
        (clause.state1 ? "":"-") << clause.var1 + 1 << ", " <<
        (clause.state2 ? "":"-") << clause.var2 + 1 << ", " <<
        (clause.state3 ? "":"-") << clause.var3 + 1 << ")";
}
