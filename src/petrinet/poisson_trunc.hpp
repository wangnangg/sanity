#include <tuple>
#include <vector>
#include "type.hpp"

namespace sanity::petrinet
{
std::tuple<uint, uint, std::vector<Real>> poissonTruncPoint(Real lambda,
                                                            Real tol);
}
