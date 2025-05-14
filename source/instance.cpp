#include "instance.h"

std::ostream& operator<<(std::ostream& c, std::unique_ptr<Instance>& i) { return i->cout(c); }
