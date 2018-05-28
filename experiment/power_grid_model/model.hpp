#pragma once
#include <iostream>
#include "linear.hpp"
#include "petrinet.hpp"
#include "powerflow.hpp"
#include "splinear.hpp"

using namespace sanity::petrinet;
using namespace sanity::linear;
using namespace sanity::splinear;
using namespace sanity::powerflow;

enum class ExpBusType
{
    PQ,
    PV
};

struct ExpBus
{
    uint idx;
    Real generation;
    Real genRatio;
    Real load;
    ExpBusType type;
    bool busOk;
    bool loadOk;
    bool loadConnected;
    bool generatorOk;
};

struct ExpLine
{
    uint idx;
    Real reactance;
    uint startBus;
    uint endBus;
    Real maxPower;
    bool ok;
};

struct ExpPowerFlowModel
{
    std::vector<ExpBus> buses;  // 0, 1, 2, ...
    std::vector<ExpLine> lines;
    uint nbus;
    uint nload;
    uint ngen;
    uint nline;

    ExpPowerFlowModel()
        : buses(), lines(), nbus(0), nload(0), ngen(0), nline(0)
    {
    }

    uint addBus(ExpBusType type, Real load, Real generation)
    {
        nbus += 1;
        if (load > 0)
        {
            nload += 1;
        }
        if (generation > 0)
        {
            ngen += 1;
        }
        uint idx = buses.size();
        buses.push_back({.idx = idx,
                         .generation = generation,
                         .genRatio = 1.0,
                         .load = load,
                         .type = type,
                         .busOk = true,
                         .loadOk = true,
                         .loadConnected = true,
                         .generatorOk = true}

        );
        return idx;
    }
    uint addLine(uint start, uint end, Real reactance, Real maxPower)
    {
        nline += 1;
        uint idx = lines.size();
        lines.push_back({.idx = idx,
                         .reactance = reactance,
                         .startBus = start,
                         .endBus = end,
                         .maxPower = maxPower,
                         .ok = true});
        return idx;
    }
};

struct ConversionResult
{
    DCPowerFlowModel dcModel;
    Permutation exp2dcBus;
    Permutation exp2dcLine;
};

ConversionResult exp2DCModel(const ExpPowerFlowModel& exp_model);

ExpPowerFlowModel ieeeCdfModel2ExpModel(const IeeeCdfModel& model,
                                        Real max_factor);
