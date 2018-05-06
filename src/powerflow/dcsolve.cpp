#include "dcsolve.hpp"
#include "graph.hpp"
#include "linear.hpp"

namespace sanity::powerflow
{
using namespace graph;
using namespace linear;

static Graph pf2graph(const DCPowerFlowModel& model)
{
    Graph g(model.busCount());
    for (uint i = 0; i < model.lineCount(); i++)
    {
        const auto& line = model.getLine(i);
        g.addEdge(line.startBus, line.endBus);
    }
    return g;
}

Matrix admittanceMatrixB(const DCPowerFlowModel& model,
                         const std::vector<uint>& bus_idx)
{
    Permutation bus2matrix(bus_idx, false);
    std::vector<uint> lines;
    uint n = bus_idx.size();
    Matrix B((int)n, (int)n, 0);
    for (uint line_i = 0; line_i < model.lineCount(); line_i++)
    {
        const auto& line = model.getLine(line_i);
        if ((bus2matrix.forward(line.startBus) < 0) ||
            bus2matrix.forward(line.endBus) < 0)
        {
            continue;
        }
        int i = bus2matrix.forward(line.startBus);
        int j = bus2matrix.forward(line.endBus);
        Real b = -1.0 / line.reactance;
        B(i, j) = b;
        B(j, i) = b;
        B(i, i) -= b;
        B(j, j) -= b;
    }
    return B;
}

int electSlack(const DCPowerFlowModel& model, const std::vector<uint>& buses)
{
    int max_i = -1;
    Real max_inj_power;
    bool first = true;
    for (uint i = 0; i < buses.size(); i++)
    {
        uint bus_idx = buses[i];
        const auto& bus = model.getBus(bus_idx);
        if (model.getBus(bus_idx).type == DCBus::Generator)
        {
            if (first || bus.injectedRealPower > max_inj_power)
            {
                max_inj_power = bus.injectedRealPower;
                first = false;
                max_i = (int)i;
            }
        }
    }
    return max_i;
}

DCPowerFlowResult solveDC(
    const DCPowerFlowModel& model,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView xb)>& linear_solver)

{
    auto graph = pf2graph(model);
    auto components = connectedComponents(graph);
    auto bus_angles = std::vector<Real>(model.busCount());
    auto slack_powers = std::vector<Real>(components.size());
    for (uint comp_i = 0; comp_i < components.size(); comp_i++)
    {
        auto& comp = components[comp_i];
        uint n = comp.size();
        int slack_idx = electSlack(model, comp);
        assert(slack_idx >= 0);  // no generator?
        std::swap(comp[0], comp[(uint)slack_idx]);
        auto B = admittanceMatrixB(model, comp);
        std::cout << B << std::endl;
        Vector angles((int)n);
        for (uint i = 1; i < n; i++)
        {
            angles((int)i) = model.getBus(comp[i]).injectedRealPower;
        }
        linear_solver(blockView(mutableView(B), 1, 1, -1, -1),
                      blockView(mutableView(angles), 1, -1));
        angles(0) = 0;
        for (uint i = 0; i < n; i++)
        {
            auto bus_idx = comp[i];
            bus_angles[bus_idx] = angles((int)i);
        }
        slack_powers[comp_i] = dot(rowView(B, 0), angles);
    }

    // compute line powers
    auto line_powers = std::vector<Real>(model.lineCount());
    for (uint i = 0; i < model.lineCount(); i++)
    {
        const auto& line = model.getLine(i);
        Real angle_diff = bus_angles[line.startBus] - bus_angles[line.endBus];
        line_powers[i] = 1.0 / line.reactance * (angle_diff);
    }

    return {.busVoltageAngles = std::move(bus_angles),
            .lineRealPowers = std::move(line_powers),
            .slackBusRealPower = std::move(slack_powers),
            .islands = std::move(components)};
}

}  // namespace sanity::powerflow
