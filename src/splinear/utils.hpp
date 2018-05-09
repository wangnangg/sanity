#pragma once

#include "matrix.hpp"

namespace sanity::splinear
{
std::ostream& operator<<(std::ostream& os, const Spmatrix& spmat);
std::ostream& operator<<(std::ostream& os, const SpmatrixTriple& spmat);

}  // namespace sanity::splinear
