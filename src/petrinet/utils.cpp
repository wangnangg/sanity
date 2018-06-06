#include "utils.hpp"

namespace sanity::petrinet
{
std::ostream& operator<<(std::ostream& os, const MarkingIntf& mk)
{
    os << "( ";
    for (uint i = 0; i < mk.size(); i++)
    {
        os << mk.nToken(i) << " ";
    }
    os << ")";
    return os;
}

}  // namespace sanity::petrinet
