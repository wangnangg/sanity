#pragma once
#include <functional>
#include <vector>
#include "linear.hpp"
#include "model.hpp"
#include "type.hpp"

namespace sanity::powerflow
{
class BusMap
{
    std::vector<int> _grid2mat;
    std::vector<int> _mat2grid;

public:
    BusMap(std::vector<int> grid2mat, std::vector<int> mat2grid)
        : _grid2mat(std::move(grid2mat)), _mat2grid(std::move(mat2grid))
    {
    }

    int getGridIdx(int mat_idx) const { return _mat2grid[(uint)mat_idx]; }
    int getMatrixIdx(int grid_idx) const { return _grid2mat[(uint)grid_idx]; }
};

BusMap remapBus(const PowerGrid& grid, int slackBus);
linear::CMatrix admittanceMatrix(const PowerGrid& grid,
                                 const BusMap& bus_map);
void fullJacobian(linear::VectorConstView VAngle,
                  linear::VectorConstView VAmp, linear::VectorConstView P,
                  linear::VectorConstView Q, linear::CMatrixConstView Y,
                  linear::MatrixMutableView jac);

struct BusState
{
    Complex power;
    Complex voltage;
};

std::vector<BusState> flatStart(const PowerGrid& grid, int slackBusIdx);

// sol is the init guess, which will be overwritten with the final solution.
IterationResult solveNewton(
    const PowerGrid& grid, int slackBusIdx, std::vector<BusState>& sol,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView xb)>& linear_solver,
    int max_iter, Real tol);

}  // namespace sanity::powerflow
