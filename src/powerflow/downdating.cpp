#include "downdating.hpp"
#include "linear.hpp"
using namespace sanity::linear;

namespace sanity::powerflow
{
void loadSolution(const BusMap& map, const std::vector<BusState>& sol,
                  VectorMutableView VAngle, VectorMutableView VAmp,
                  VectorMutableView Px, VectorMutableView Qx)
{
    int n = sol.size();
    for (int i = 0; i < n; i++)
    {
        int gridIdx = map.getGridIdx(i);
        const auto& state = sol[(uint)gridIdx];
        VAmp(i) = std::abs(state.voltage);
        VAngle(i) = std::arg(state.voltage);
        Px(i) = state.power.real();
        Qx(i) = state.power.imag();
    }
}

static void output(const BusMap& map, VectorConstView VAngle,
                   VectorConstView VAmp, VectorConstView P, VectorConstView Q,
                   std::vector<BusState>& sol)
{
    int n = sol.size();
    for (int i = 0; i < n; i++)
    {
        int bus_i = map.getGridIdx(i);
        sol[(uint)bus_i].power = Complex(P(i), Q(i));
        sol[(uint)bus_i].voltage = std::polar(VAmp(i), VAngle(i));
    }
}

// slack -- generator line
void downdatingOnDisconnectedLineSG(const PowerGrid& grid, int slackBusIdx,
                                    int lineIdx, std::vector<BusState>& sol)
{
    int n = grid.busCount();
    int nl = grid.loadCount();
    int ng = n - nl;

    auto bus_map = remapBus(grid, slackBusIdx);
    const auto& line = grid.getLine(lineIdx);
    int gbus_j;
    if (line.startBus == slackBusIdx)
    {
        gbus_j = bus_map.getMatrixIdx(line.endBus);
    }
    else
    {
        gbus_j = bus_map.getMatrixIdx(line.startBus);
    }

    auto Y = admittanceMatrix(grid, bus_map);
    auto VAngle = Vector(n);
    auto VAmp = Vector(n);
    auto Px = Vector(n);
    auto Qx = Vector(n);
    auto x = Vector(n - 1 + nl);  // n - 1 angles and nl amplitudes
    auto jac_f_x = Matrix(n - 1 + nl, n - 1 + nl);
    auto _jac_f_a = Matrix(n - 1 + nl, 3, 0.0);  // - jac_f_a
    auto da = Vector(3);
    loadSolution(bus_map, sol, mutableView(VAngle), mutableView(VAmp),
                 mutableView(Px), mutableView(Qx));

    fullJacobian(VAngle, VAmp, Px, Qx, Y, mutableView(jac_f_x));

    // G_0j
    _jac_f_a(gbus_j - 1, 0) =
        VAmp(gbus_j) * VAmp(0) * std::cos(VAngle(gbus_j) - VAngle(0));

    // B_0j
    _jac_f_a(gbus_j - 1, 1) =
        VAmp(gbus_j) * VAmp(0) * std::sin(VAngle(gbus_j) - VAngle(0));

    // G_jj
    _jac_f_a(gbus_j - 1, 2) = VAmp(gbus_j) * VAmp(gbus_j);

    scale(-1.0, mutableView(_jac_f_a));

    // solve jac_f_x . jac_x_a = _jac_f_a
    lapack::gesv(mutableView(jac_f_x), mutableView(_jac_f_a));
    // now, jac_x_a is solved.
    auto jac_x_a = _jac_f_a;

    Complex y0j = 1.0 / line.totalImped;
    // G_0j
    da(0) = y0j.real();

    // B_0j
    da(1) = y0j.imag();

    // G_jj
    da(2) = -y0j.real();

    // copy solution to x
    for (int i = 0; i < n - 1; i++)
    {
        x(i) = VAngle(i + 1);
    }
    for (int i = 0; i < nl; i++)
    {
        x(i + n - 1) = VAmp(ng + i);
    }
    // x = jac_x_a * da + x
    blas::gemv(1.0, jac_x_a, blas::NoTranspose, da, 1.0, mutableView(x));

    // copy it back
    for (int i = 0; i < n - 1; i++)
    {
        VAngle(i + 1) = x(i);
    }
    for (int i = 0; i < nl; i++)
    {
        VAmp(ng + i) = x(i + n - 1);
    }

    output(bus_map, VAngle, VAmp, Px, Qx, sol);
}

}  // namespace sanity::powerflow
