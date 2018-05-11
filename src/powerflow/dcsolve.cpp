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
    Matrix B(n, n, 0);
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
        assert(i >= 0);
        assert(j >= 0);
        B((uint)i, (uint)j) = b;
        B((uint)j, (uint)i) = b;
        B((uint)i, (uint)i) -= b;
        B((uint)j, (uint)j) -= b;
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
        if (model.getBus(bus_idx).type == DCBusType::PV)
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
    auto components = decompCc(graph);
    auto bus_angles = std::vector<Real>(model.busCount());
    auto slack_powers = std::vector<Real>(components.size());
    std::vector<bool> solved(components.size(), true);
    for (uint comp_i = 0; comp_i < components.size(); comp_i++)
    {
        auto& comp = components[comp_i];
        uint n = comp.nodes.size();
        int slack_idx = electSlack(model, comp.nodes);
        if (slack_idx < 0)  // no generator bus is found
        {
            for (uint i = 0; i < n; i++)
            {
                auto bus_idx = comp.nodes[i];
                if (model.getBus(bus_idx).injectedRealPower !=
                    0)  // impossible
                {
                    solved[comp_i] = false;
                }
                bus_angles[bus_idx] = 0.0;
            }
            slack_powers[comp_i] = 0.0;
            continue;
        }
        if (comp.nodes.size() == 1)
        {
            // nothing to solve here
            auto bus_idx = comp.nodes[0];
            bus_angles[bus_idx] = 0;
            slack_powers[comp_i] = -model.getBus(bus_idx).injectedRealPower;
            continue;
        }
        std::swap(comp.nodes[0], comp.nodes[(uint)slack_idx]);
        auto B = admittanceMatrixB(model, comp.nodes);
        Vector angles(n);
        for (uint i = 1; i < n; i++)
        {
            angles(i) = model.getBus(comp.nodes[i]).injectedRealPower;
        }
        std::cout << B << std::endl;
        linear_solver(
            blockView(mutableView(B), 1, 1, B.nrow() - 1, B.ncol() - 1),
            blockView(mutableView(angles), 1, angles.size() - 1));
        angles(0) = 0;
        for (uint i = 0; i < n; i++)
        {
            auto bus_idx = comp.nodes[i];
            bus_angles[bus_idx] = angles(i);
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

    std::vector<std::vector<uint>> islands;
    for (const auto& comp : components)
    {
        islands.push_back(std::move(comp.nodes));
    }
    return {.busVoltageAngles = std::move(bus_angles),
            .lineRealPowers = std::move(line_powers),
            .slackBusRealPower = std::move(slack_powers),
            .islands = std::move(islands)};
}

}  // namespace sanity::powerflow
