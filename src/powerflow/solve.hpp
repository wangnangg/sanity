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
    std::vector<uint> _grid2mat;
    std::vector<uint> _mat2grid;

public:
    BusMap(std::vector<uint> grid2mat, std::vector<uint> mat2grid)
        : _grid2mat(std::move(grid2mat)), _mat2grid(std::move(mat2grid))
    {
    }

    uint getGridIdx(uint mat_idx) const { return _mat2grid[mat_idx]; }
    uint getMatrixIdx(uint grid_idx) const { return _grid2mat[grid_idx]; }
};

BusMap remapBus(const PowerGrid& grid, uint slackBus);
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

std::vector<BusState> flatStart(const PowerGrid& grid, uint slackBusIdx);

// sol is the init guess, which will be overwritten with the final solution.
IterationResult solveNewton(
    const PowerGrid& grid, uint slackBusIdx, std::vector<BusState>& sol,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView xb)>& linear_solver,
    uint max_iter, Real tol);

}  // namespace sanity::powerflow
