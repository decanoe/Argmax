#include "reversible_instance.h"

std::ostream& operator<<(std::ostream& c, std::unique_ptr<ReversibleInstance>& i) { return i->cout(c); }
