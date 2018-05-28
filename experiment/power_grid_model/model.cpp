#include "model.hpp"

ConversionResult exp2DCModel(const ExpPowerFlowModel& exp_model)
{
    DCPowerFlowModel dc_model;
    std::vector<uint> dc2exp_bus;
    std::vector<uint> dc2exp_line;

    for (uint i = 0; i < exp_model.buses.size(); i++)
    {
        auto& bus = exp_model.buses[i];
        if (!bus.busOk)
        {
            continue;
        }
        Real load = bus.load;
        Real gen = bus.generation * bus.genRatio;
        DCBusType type =
            bus.type == ExpBusType::PQ ? DCBusType::PQ : DCBusType::PV;
        if (!bus.loadConnected || !bus.loadOk)
        {  //
            load = 0.0;
        }
        if (!bus.generatorOk)
        {
            type = DCBusType::PQ;
            gen = 0.0;
        }
        dc_model.addBus(type, gen - load);
        dc2exp_bus.push_back(i);
    }
    Permutation exp2dc_bus(dc2exp_bus, false);
    for (uint i = 0; i < exp_model.lines.size(); i++)
    {
        auto& line = exp_model.lines[i];
        if (!line.ok)
        {
            continue;
        }
        auto dc_start = exp2dc_bus.forward(line.startBus);
        auto dc_end = exp2dc_bus.forward(line.endBus);
        if (dc_start < 0 || dc_end < 0)
        {
            // line is not connected
            continue;
        }
        dc_model.addTransmissionLine((uint)dc_start, (uint)dc_end,
                                     line.reactance);
        dc2exp_line.push_back(i);
    }
    Permutation exp2dc_line(dc2exp_line, false);
    return {.dcModel = std::move(dc_model),
            .exp2dcBus = std::move(exp2dc_bus),
            .exp2dcLine = std::move(exp2dc_line)};
}

ExpPowerFlowModel ieeeCdfModel2ExpModel(const IeeeCdfModel& model,
                                        Real max_factor)
{
    ExpPowerFlowModel exp_model;
    for (const auto& bus : model.buses)
    {
        Real load = bus.loadP_MW / model.MVABase;
        Real gen = bus.genP_MW / model.MVABase;
        switch (bus.type)
        {
            case IeeeCdfLoad:
            case IeeeCdfGenPQ:
                exp_model.addBus(ExpBusType::PQ, load, gen);
                break;
            case IeeeCdfGenPV:
                exp_model.addBus(ExpBusType::PV, load, gen);
                break;
            case IeeeCdfSlack:
                if (gen <= 0.0)
                {
                    gen = 1.0;
                }
                exp_model.addBus(ExpBusType::PV, load, gen);
                break;
        }
    }
    for (const auto& branch : model.branches)
    {
        exp_model.addLine((uint)(branch.tapBus - 1), (uint)(branch.zBus - 1),
                          branch.reactanceX_PU, 1.0);
    }

    // set up line max
    auto converted = exp2DCModel(exp_model);
    auto dc_res = solveDC(converted.dcModel, solveLU);
    for (bool s : dc_res.islandSolved)
    {
        assert(s);
    }
    for (auto& line : exp_model.lines)
    {
        int dc_line_idx = converted.exp2dcLine.forward(line.idx);
        assert(dc_line_idx >= 0);
        Real line_power = dc_res.lineRealPowers[(uint)dc_line_idx];
        line.maxPower = std::max(std::abs(line_power * max_factor), 1.0);
    }
    return exp_model;
}
