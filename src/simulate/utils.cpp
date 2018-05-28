#include "utils.hpp"

namespace sanity::simulate
{
std::ostream& operator<<(std::ostream& os, const Interval& itv)
{
    os << "[" << itv.begin << ", " << itv.end << "]";
    return os;
}
}  // namespace sanity::simulate
