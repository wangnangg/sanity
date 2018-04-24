#include "solve.hpp"
#include <cassert>
#include <cmath>
#include "linear.hpp"

using namespace sanity::linear;

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

// generators come first and then loads, index 0 is the slack bus.
BusMap remapBus(const PowerGrid& grid, int slackBus)
{
    auto mat2grid = std::vector<int>((uint)grid.busCount());
    auto grid2mat = std::vector<int>((uint)grid.busCount());
    int gen_idx = 1;
    int load_idx = grid.generatorCount();
    for (int i = 0; i < grid.busCount(); i++)
    {
        const auto& bus = grid.getBus(i);
        if (bus.type == GeneratorBus)
        {
            if (bus.idx == slackBus)
            {
                mat2grid[0] = bus.idx;
                grid2mat[(uint)bus.idx] = 0;
            }
            else
            {
                mat2grid[(uint)gen_idx] = bus.idx;
                grid2mat[(uint)bus.idx] = gen_idx;
                gen_idx += 1;
            }
        }
        else
        {  // load bus
            mat2grid[(uint)load_idx] = bus.idx;
            grid2mat[(uint)bus.idx] = load_idx;
            load_idx += 1;
        }
    }
    return BusMap(std::move(grid2mat), std::move(mat2grid));
}

CMatrix admittanceMatrix(const PowerGrid& grid, const BusMap& bus_map)
{
    int n = grid.busCount();
    CMatrix mat(n, n, Complex());
    for (int i = 0; i < grid.lineCount(); i++)
    {
        const auto& line = grid.getLine(i);
        int start = bus_map.getMatrixIdx(line.startBus);
        int end = bus_map.getMatrixIdx(line.endBus);
        Complex y = Complex(1.0, 0.0) / line.totalImped;

        mat(start, end) -= y;
        mat(end, start) -= y;

        mat(start, start) += y;
        mat(start, start) += Complex(0.0, line.shuntSusceptance);

        mat(end, end) += y;
        mat(end, end) += Complex(0.0, line.shuntSusceptance);
    }
    for (int i = 0; i < grid.shuntCount(); i++)
    {
        const auto& shunt = grid.getShunt(i);
        int bus = bus_map.getMatrixIdx(shunt.busIdx);
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
    int n = VAngle.size();
    assert(n == VAmp.size());
    assert(Y.nrow() == Y.ncol());
    assert(n == Y.nrow());
    assert(n == P.size());
    assert(n == Q.size());
    // real power
    for (int k = 0; k < P.size(); k++)
    {
        P(k) = 0;
        for (int j = 0; j < n; j++)
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
    for (int k = 0; k < Q.size(); k++)
    {
        Q(k) = 0;
        for (int j = 0; j < n; j++)
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
    int n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(n - 1 == jac.nrow());
    assert(n - 1 == jac.ncol());
    for (int j = 0; j < jac.nrow(); j++)  // bus index
    {
        for (int k = 0; k < jac.ncol(); k++)  // bus index
        {
            int bus_j = j + 1;
            int bus_k = k + 1;
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
    int n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(n - 1 == jac.nrow());
    int nl = jac.ncol();
    int ng = n - nl;
    for (int j = 0; j < jac.nrow(); j++)
    {
        for (int k = 0; k < jac.ncol(); k++)
        {
            int bus_j = j + 1;
            int bus_k = k + ng;  // first load
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
    int n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(n - 1 == jac.ncol());
    int nl = jac.nrow();
    int ng = n - nl;

    for (int j = 0; j < jac.nrow(); j++)
    {
        for (int k = 0; k < jac.ncol(); k++)
        {
            int bus_j = j + ng;  // first load
            int bus_k = k + 1;
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
    int n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(jac.nrow() == jac.ncol());
    int nl = jac.nrow();
    int ng = n - nl;

    for (int j = 0; j < jac.nrow(); j++)
    {
        for (int k = 0; k < jac.ncol(); k++)
        {
            int bus_j = j + ng;  // first load
            int bus_k = k + ng;  // first load
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
    int n = VAngle.size();
    assert(n == VAmp.size());
    assert(n == P.size());
    assert(n == Q.size());
    assert(n == Y.nrow());
    assert(n == Y.ncol());
    assert(jac.nrow() == jac.ncol());
    int nl = jac.nrow() + 1 - n;
    jacPAngle(VAngle, VAmp, P, Q, Y, blockView(jac, 0, 0, n - 1, n - 1));
    jacPAmp(VAngle, VAmp, P, Q, Y, blockView(jac, 0, n - 1, n - 1, nl));
    jacQAngle(VAngle, VAmp, P, Q, Y, blockView(jac, n - 1, 0, nl, n - 1));
    jacQAmp(VAngle, VAmp, P, Q, Y, blockView(jac, n - 1, n - 1, nl, nl));
}

std::vector<BusState> flatStart(const PowerGrid& grid, int slackBusIdx)
{
    std::vector<BusState> states((uint)grid.busCount());
    Real defVol = grid.getBus(slackBusIdx).attr.generator.voltageAmp;
    for (int i = 0; i < (int)states.size(); i++)
    {
        const auto& bus = grid.getBus(i);
        switch (bus.type)
        {
            case GeneratorBus:  // PV
                states[(uint)i].power =
                    Complex(bus.attr.generator.realPower, 0);
                states[(uint)i].voltage =
                    Complex(bus.attr.generator.voltageAmp, 0);
                break;
            case LoadBus:
                states[(uint)i].power = bus.attr.load.power;
                states[(uint)i].voltage = Complex(defVol, 0);
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

    int ng = grid.generatorCount();
    int n = grid.busCount();
    // for other generators, PV bus
    for (int i = 1; i < ng; i++)
    {
        int gridIdx = map.getGridIdx(i);
        const auto& bus = grid.getBus(gridIdx);
        P(i) = bus.attr.generator.realPower;
        VAmp(i) = bus.attr.generator.voltageAmp;
        Q(i) = guess[(uint)gridIdx].power.imag();
        VAngle(i) = std::arg(guess[(uint)gridIdx].voltage);
    }
    // for loads, PQ bus
    for (int i = ng; i < n; i++)
    {
        int gridIdx = map.getGridIdx(i);
        const auto& bus = grid.getBus(gridIdx);
        P(i) = -bus.attr.load.power.real();  // injected power
        Q(i) = -bus.attr.load.power.imag();  // injected power
        VAmp(i) = std::abs(guess[(uint)gridIdx].voltage);
        VAngle(i) = std::arg(guess[(uint)gridIdx].voltage);
    }
}

void powerMismatch(
    VectorConstView P, VectorConstView Q, VectorConstView Px,
    VectorConstView Qx,
    VectorMutableView residual  // n - 1 real power and  nl reactive power
)
{
    int n = P.size();
    int nl = residual.size() - n + 1;
    int ng = n - nl;
    for (int i = 0; i < n - 1; i++)
    {
        residual(i) = Px(i + 1) - P(i + 1);
    }
    for (int i = 0; i < nl; i++)
    {
        residual(i + n - 1) = Qx(ng + i) - Q(ng + i);
    }
}

static void update(VectorConstView _dx, VectorMutableView VAngle,
                   VectorMutableView VAmp)
{
    int n = VAngle.size();
    int nl = _dx.size() - n + 1;
    int ng = n - nl;
    blas::axpy(-1.0, blockView(_dx, 0, n - 1),
               blockView(VAngle, 1, n - 1));  // update  n - 1 angles
    blas::axpy(-1.0, blockView(_dx, n - 1, nl),
               blockView(VAmp, ng, nl));  // update nl amplitudes for loads
}

static void output(const PowerGrid& grid, const BusMap& map,
                   VectorConstView VAngle, VectorConstView VAmp,
                   VectorConstView P, VectorConstView Q,
                   std::vector<BusState>& sol)
{
    int n = sol.size();
    for (int i = 0; i < n; i++)
    {
        int bus_i = map.getGridIdx(i);
        const auto& bus = grid.getBus(bus_i);
        if (bus.type == GeneratorBus)
        {
            sol[(uint)bus_i].power = Complex(P(i), Q(i));
        }
        else
        {  // load
            sol[(uint)bus_i].power = Complex(P(i), Q(i));
        }
        sol[(uint)bus_i].voltage = std::polar(VAmp(i), VAngle(i));
    }
}

IterationResult newton(
    const PowerGrid& grid, int slackBusIdx, std::vector<BusState>& sol,
    const std::function<void(MatrixMutableView A, VectorMutableView xb)>&
        linear_solver,
    int max_iter, Real tol)
{
    assert(grid.getBus(slackBusIdx).type == GeneratorBus);
    int n = grid.busCount();
    int nl = grid.loadCount();

    auto bus_map = remapBus(grid, slackBusIdx);
    auto Y = admittanceMatrix(grid, bus_map);
    std::cout << "Y:\n" << Y << std::endl;
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
    int iter;
    for (iter = 0; iter < max_iter; iter++)
    {
        powerFromVoltage(VAngle, VAmp, Y, mutableView(Px), mutableView(Qx));
        std::cout << "Px: " << Px << std::endl;
        std::cout << "P: " << P << std::endl;
        std::cout << "Qx: " << Qx << std::endl;
        std::cout << "Q: " << Q << std::endl;
        powerMismatch(P, Q, Px, Qx, mutableView(residual));
        error = maxAbs(residual);
        if (error < tol)
        {
            break;
        }
        fullJacobian(VAngle, VAmp, Px, Qx, Y, mutableView(jac));
        std::cout << "J:\n" << jac << std::endl;
        // dx = - jac^(-1) res, i.e. jac (- dx) = res
        std::cout << "residual:\n" << residual << std::endl;
        linear_solver(mutableView(jac), mutableView(residual));
        // now -dx = residual
        std::cout << "-dx:\n" << residual << std::endl;
        update(residual, mutableView(VAngle), mutableView(VAmp));
    }
    output(grid, bus_map, VAngle, VAmp, Px, Qx, sol);
    return IterationResult{.error = error, .nIter = iter};
}

}  // namespace sanity::powerflow
