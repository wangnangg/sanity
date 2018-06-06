#pragma once
#include <ostream>
#include "petrinet.hpp"

namespace sanity::petrinet
{
std::ostream& operator<<(std::ostream& os, const MarkingIntf& mk);
}
