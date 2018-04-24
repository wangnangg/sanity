#include <functional>
#include <vector>
#include "linear.hpp"
#include "model.hpp"
#include "type.hpp"

namespace sanity::powerflow
{
struct BusState
{
    Complex power;
    Complex voltage;
};

std::vector<BusState> flatStart(const PowerGrid& grid, int slackBusIdx);

// sol is the init guess, which will be overwritten with the final solution.
IterationResult newton(
    const PowerGrid& grid, int slackBusIdx, std::vector<BusState>& sol,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView xb)>& linear_solver,
    int max_iter, Real tol);

}  // namespace sanity::powerflow
