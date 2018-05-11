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

ExpPowerFlowModel ieeeCdfModel2ExpModel(const IeeeCdfModel& model)
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
                    gen = 1000.0;
                }
                exp_model.addBus(ExpBusType::PV, load, gen);
                break;
        }
    }
    for (const auto& branch : model.branches)
    {
        exp_model.addLine((uint)(branch.tapBus - 1), (uint)(branch.zBus - 1),
                          branch.reactanceX_PU, 0);
    }
    return exp_model;
}
