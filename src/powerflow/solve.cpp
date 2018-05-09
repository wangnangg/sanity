#include "solve.hpp"
#include <cassert>
#include <cmath>
#include "linear.hpp"

using namespace sanity::linear;

namespace sanity::powerflow
{
// generators come first and then loads, index 0 is the slack bus.
BusMap remapBus(const PowerGrid& grid, uint slackBus)
{
    auto mat2grid = std::vector<uint>(grid.busCount());
    auto grid2mat = std::vector<uint>(grid.busCount());
    uint gen_idx = 1;
    uint load_idx = grid.generatorCount();
    for (uint i = 0; i < grid.busCount(); i++)
    {
        const auto& bus = grid.getBus(i);
        if (bus.type == GeneratorBus)
        {
            if (bus.idx == slackBus)
            {
                mat2grid[0] = bus.idx;
                grid2mat[bus.idx] = 0;
            }
            else
            {
                mat2grid[gen_idx] = bus.idx;
                grid2mat[bus.idx] = gen_idx;
                gen_idx += 1;
            }
        }
        else
        {  // load bus
            mat2grid[load_idx] = bus.idx;
            grid2mat[bus.idx] = load_idx;
            load_idx += 1;
        }
    }
    return BusMap(std::move(grid2mat), std::move(mat2grid));
}

CMatrix admittanceMatrix(const PowerGrid& grid, const BusMap& bus_map)
{
    uint n = grid.busCount();
    CMatrix mat(n, n, Complex());
    for (uint i = 0; i < grid.lineCount(); i++)
    {
        const auto& line = grid.getLine(i);
        uint start = (uint)bus_map.getMatrixIdx(line.startBus);
        uint end = (uint)bus_map.getMatrixIdx(line.endBus);
        Complex y = Complex(1.0, 0.0) / line.totalImped;

        mat(start, end) -= y;
        mat(end, start) -= y;

        mat(start, start) += y;
        mat(start, start) += Complex(0.0, line.shuntSusceptance);

        mat(end, end) += y;
        mat(end, end) += Complex(0.0, line.shuntSusceptance);
    }
    for (uint i = 0; i < grid.shuntCount(); i++)
    {
        const auto& shunt = grid.getShunt(i);
        uint bus = bus_map.getMatrixIdx(shunt.busIdx);
        mat(bus, bus) += Complex(1.0, 0.0) / shunt.impedance;
    }
    return mat;
}
/* VAngle: Angles of buses 0, 1, 2, ..., N - 1
 * VAmp: Voltage amplitudes of buses 0, 1, 2, ..., N - 1
 * Y: N * N admittance Matrix
 * P: real power of length N
 * Q: reactive power of length N
 */
void powerFromVoltage(VectorConstView VAngle, VectorConstView VAmp,
                      CMatrixConstView Y, VectorMutableView P,
                      VectorMutableView Q)
{
    uint n = VAngle.size();
    assert(n == VAmp.size());
    assert(Y.nrow() == Y.ncol());
    assert(n == Y.nrow());
    assert(n == P.size());
    assert(n == Q.size());
    // real power
    for (uint k = 0; k < P.size(); k++)
    {
        P(k) = 0;
        for (uint j = 0; j < n; j++)
        {
            Real G = Y(k, j).real();
            Real B = Y(k, j).imag();
            if (G != 0.0 || B != 0.0)
            {
                P(k) += VAmp(k) * VAmp(j) *
                        (G * std::cos(VAngle(k) - VAngle(j)) +
                         B * std::sin(VAngle(k) - VAngle(j)));
            }
        }
    }

    // reactive power
    for (uint k = 0; k < Q.size(); k++)
    {
        Q(k) = 0;
        for (uint j = 0; j < n; j++)
        {
            Real G = Y(k, j).real();
            Real B = Y(k, j).imag();
            if (G != 0.0 || B != 0.0)
            {
                Q(k) += VAmp(k) * VAmp(j) *
                        (G * std::sin(VAngle(k) - VAngle(j)) -
                         B * std::cos(VAngle(k) - VAngle(j)));
            }
        }
    }
}

/* Jac: Jacobian matrix of (N - 1) * (N - 1) between P and Angle, excluding
 * the slack
 *
 * VAngle: voltage angles of N
 *
 * VAmp: voltage amplitudes of N
 *
 * P: power of N
 *
 * Q: power of N
 *
 * Y: admittance Matrix of N * N
 */
void jacPAngle(VectorConstView VAngle, VectorConstView VAmp,
               VectorConstView P, VectorConstView Q, CMatrixConstView Y,
               MatrixMutableView jac)
{
    uint n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(n - 1 == jac.nrow());
    assert(n - 1 == jac.ncol());
    for (uint j = 0; j < jac.nrow(); j++)  // bus index
    {
        for (uint k = 0; k < jac.ncol(); k++)  // bus index
        {
            uint bus_j = j + 1;
            uint bus_k = k + 1;
            Real G = Y(bus_j, bus_k).real();
            Real B = Y(bus_j, bus_k).imag();
            if (bus_j == bus_k)
            {
                jac(j, k) = -Q(bus_j) - B * VAmp(bus_j) * VAmp(bus_j);
            }
            else
            {
                jac(j, k) = VAmp(bus_j) * VAmp(bus_k) *
                            (G * std::sin(VAngle(bus_j) - VAngle(bus_k)) -
                             B * std::cos(VAngle(bus_j) - VAngle(bus_k)));
            }
        }
    }
}

/* Jac: Jacobian matrix of (N - 1) * NL between P and Voltage Amplitudes of
 * load buses
 *
 * VAngle: voltage angles of N
 *
 * VAmp: voltage amplitudes of N
 *
 * P: power of N
 *
 * Q: power of N
 *
 * Y: admittance Matrix of N * N
 */
void jacPAmp(VectorConstView VAngle, VectorConstView VAmp, VectorConstView P,
             VectorConstView Q, CMatrixConstView Y, MatrixMutableView jac)
{
    uint n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(n - 1 == jac.nrow());
    uint nl = jac.ncol();
    uint ng = n - nl;
    for (uint j = 0; j < jac.nrow(); j++)
    {
        for (uint k = 0; k < jac.ncol(); k++)
        {
            uint bus_j = j + 1;
            uint bus_k = k + ng;  // first load
            Real G = Y(bus_j, bus_k).real();
            Real B = Y(bus_j, bus_k).imag();
            if (bus_j == bus_k)
            {
                jac(j, k) = P(bus_j) / VAmp(bus_j) + G * VAmp(bus_j);
            }
            else
            {
                jac(j, k) = VAmp(bus_j) *
                            (G * std::cos(VAngle(bus_j) - VAngle(bus_k)) +
                             B * std::sin(VAngle(bus_j) - VAngle(bus_k)));
            }
        }
    }
}

/* Jac: Jacobian matrix of NL * N-1 between Q and Voltage Angles of buses
 * excluding the slack
 *
 * VAngle: voltage angles of N
 *
 * VAmp: voltage amplitudes of N
 *
 * P: power of N
 *
 * Q: power of N
 *
 * Y: admittance Matrix of N * N
 */
void jacQAngle(VectorConstView VAngle, VectorConstView VAmp,
               VectorConstView P, VectorConstView Q, CMatrixConstView Y,
               MatrixMutableView jac)
{
    uint n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(n - 1 == jac.ncol());
    uint nl = jac.nrow();
    uint ng = n - nl;

    for (uint j = 0; j < jac.nrow(); j++)
    {
        for (uint k = 0; k < jac.ncol(); k++)
        {
            uint bus_j = j + ng;  // first load
            uint bus_k = k + 1;
            Real G = Y(bus_j, bus_k).real();
            Real B = Y(bus_j, bus_k).imag();
            if (bus_j == bus_k)
            {
                jac(j, k) = P(bus_j) - G * VAmp(bus_j) * VAmp(bus_j);
            }
            else
            {
                jac(j, k) = -VAmp(bus_j) * VAmp(bus_k) *
                            (G * std::cos(VAngle(bus_j) - VAngle(bus_k)) +
                             B * std::sin(VAngle(bus_j) - VAngle(bus_k)));
            }
        }
    }
}

/* Jac: Jacobian matrix of NL * NL between Q and Voltage Amplitudes of load
 * buses
 *
 * VAngle: voltage angles of N
 *
 * VAmp: voltage amplitudes of N
 *
 * P: power of N
 *
 * Q: power of N
 *
 * Y: admittance Matrix of N * N
 */
void jacQAmp(VectorConstView VAngle, VectorConstView VAmp, VectorConstView P,
             VectorConstView Q, CMatrixConstView Y, MatrixMutableView jac)
{
    uint n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(jac.nrow() == jac.ncol());
    uint nl = jac.nrow();
    uint ng = n - nl;

    for (uint j = 0; j < jac.nrow(); j++)
    {
        for (uint k = 0; k < jac.ncol(); k++)
        {
            uint bus_j = j + ng;  // first load
            uint bus_k = k + ng;  // first load
            Real G = Y(bus_j, bus_k).real();
            Real B = Y(bus_j, bus_k).imag();
            if (bus_j == bus_k)
            {
                jac(j, k) = Q(bus_j) / VAmp(bus_j) - B * VAmp(bus_j);
            }
            else
            {
                jac(j, k) = VAmp(bus_j) *
                            (G * std::sin(VAngle(bus_j) - VAngle(bus_k)) -
                             B * std::cos(VAngle(bus_j) - VAngle(bus_k)));
            }
        }
    }
}

void fullJacobian(VectorConstView VAngle, VectorConstView VAmp,
                  VectorConstView P, VectorConstView Q, CMatrixConstView Y,
                  MatrixMutableView jac)
{
    uint n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(jac.nrow() == jac.ncol());
    uint nl = jac.nrow() + 1 - n;
    jacPAngle(VAngle, VAmp, P, Q, Y, blockView(jac, 0, 0, n - 1, n - 1));
    jacPAmp(VAngle, VAmp, P, Q, Y, blockView(jac, 0, n - 1, n - 1, nl));
    jacQAngle(VAngle, VAmp, P, Q, Y, blockView(jac, n - 1, 0, nl, n - 1));
    jacQAmp(VAngle, VAmp, P, Q, Y, blockView(jac, n - 1, n - 1, nl, nl));
}

std::vector<BusState> flatStart(const PowerGrid& grid, uint slackBusIdx)
{
    std::vector<BusState> states((uint)grid.busCount());
    Real defVol = grid.getBus(slackBusIdx).attr.generator.voltageAmp;
    for (uint i = 0; i < states.size(); i++)
    {
        const auto& bus = grid.getBus(i);
        switch (bus.type)
        {
            case GeneratorBus:  // PV
                states[i].power = Complex(bus.attr.generator.realPower, 0);
                states[i].voltage = Complex(bus.attr.generator.voltageAmp, 0);
                break;
            case LoadBus:
                states[i].power = bus.attr.load.power;
                states[i].voltage = Complex(defVol, 0);
                break;
        }
    }
    return states;
}

static void init(const PowerGrid& grid, const BusMap& map,
                 const std::vector<BusState>& guess, VectorMutableView VAngle,
                 VectorMutableView VAmp, VectorMutableView P,
                 VectorMutableView Q)
{
    // for the slack
    VAmp(0) = grid.getBus(map.getGridIdx(0)).attr.generator.voltageAmp;
    VAngle(0) = 0.0;
    P(0) = guess[(uint)map.getGridIdx(0)].power.real();
    Q(0) = guess[(uint)map.getGridIdx(0)].power.imag();

    uint ng = grid.generatorCount();
    uint n = grid.busCount();
    // for other generators, PV bus
    for (uint i = 1; i < ng; i++)
    {
        uint gridIdx = map.getGridIdx(i);
        const auto& bus = grid.getBus(gridIdx);
        P(i) = bus.attr.generator.realPower;
        VAmp(i) = bus.attr.generator.voltageAmp;
        Q(i) = guess[(uint)gridIdx].power.imag();
        VAngle(i) = std::arg(guess[(uint)gridIdx].voltage);
    }
    // for loads, PQ bus
    for (uint i = ng; i < n; i++)
    {
        uint gridIdx = map.getGridIdx(i);
        const auto& bus = grid.getBus(gridIdx);
        P(i) = -bus.attr.load.power.real();  // injected power
        Q(i) = -bus.attr.load.power.imag();  // injected power
        VAmp(i) = std::abs(guess[gridIdx].voltage);
        VAngle(i) = std::arg(guess[gridIdx].voltage);
    }
}

static void powerMismatch(
    VectorConstView P, VectorConstView Q, VectorConstView Px,
    VectorConstView Qx,
    VectorMutableView residual  // n - 1 real power and  nl reactive power
)
{
    uint n = P.size();
    uint nl = residual.size() - n + 1;
    uint ng = n - nl;
    for (uint i = 0; i < n - 1; i++)
    {
        residual(i) = Px(i + 1) - P(i + 1);
    }
    for (uint i = 0; i < nl; i++)
    {
        residual(i + n - 1) = Qx(ng + i) - Q(ng + i);
    }
}

static void update(VectorConstView _dx, VectorMutableView VAngle,
                   VectorMutableView VAmp)
{
    uint n = VAngle.size();
    uint nl = _dx.size() - n + 1;
    uint ng = n - nl;
    blas::axpy(-1.0, blockView(_dx, 0, n - 1),
               blockView(VAngle, 1, n - 1));  // update  n - 1 angles
    blas::axpy(-1.0, blockView(_dx, n - 1, nl),
               blockView(VAmp, ng, nl));  // update nl amplitudes for loads
}

static void output(const BusMap& map, VectorConstView VAngle,
                   VectorConstView VAmp, VectorConstView P, VectorConstView Q,
                   std::vector<BusState>& sol)
{
    uint n = sol.size();
    for (uint i = 0; i < n; i++)
    {
        uint bus_i = map.getGridIdx(i);
        sol[bus_i].power = Complex(P(i), Q(i));
        sol[bus_i].voltage = std::polar(VAmp(i), VAngle(i));
    }
}

IterationResult solveNewton(
    const PowerGrid& grid, uint slackBusIdx, std::vector<BusState>& sol,
    const std::function<void(MatrixMutableView A, VectorMutableView xb)>&
        linear_solver,
    uint max_iter, Real tol)
{
    assert(grid.getBus(slackBusIdx).type == GeneratorBus);
    uint n = grid.busCount();
    uint nl = grid.loadCount();

    auto bus_map = remapBus(grid, slackBusIdx);
    auto Y = admittanceMatrix(grid, bus_map);
    auto VAngle = Vector(n);
    auto VAmp = Vector(n);
    auto P = Vector(n);
    auto Q = Vector(n);
    auto Px = Vector(n);
    auto Qx = Vector(n);
    auto residual =
        Vector(n - 1 + nl);  // n - 1 real power and nl reactive power
    auto jac = Matrix(n - 1 + nl, n - 1 + nl);
    init(grid, bus_map, sol, mutableView(VAngle), mutableView(VAmp),
         mutableView(P), mutableView(Q));

    Real error = 0;
    uint iter;
    for (iter = 0; iter < (uint)max_iter; iter++)
    {
        powerFromVoltage(VAngle, VAmp, Y, mutableView(Px), mutableView(Qx));
        powerMismatch(P, Q, Px, Qx, mutableView(residual));
        error = maxAbs(residual);
        if (error < tol)
        {
            break;
        }
        fullJacobian(VAngle, VAmp, Px, Qx, Y, mutableView(jac));
        // dx = - jac^(-1) res, i.e. jac (- dx) = res
        linear_solver(mutableView(jac), mutableView(residual));
        // now -dx = residual
        update(residual, mutableView(VAngle), mutableView(VAmp));
    }
    output(bus_map, VAngle, VAmp, Px, Qx, sol);
    return IterationResult{.error = error, .nIter = iter};
}

}  // namespace sanity::powerflow
