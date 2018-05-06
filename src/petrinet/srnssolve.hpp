#pragma once

#include "linear.hpp"
#include "srn.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
IterationResult steadyStateSOR(const linear::Spmatrix& Q,
                               linear::VectorMutableView prob, Real w,
                               Real tol, uint max_iter);
}
