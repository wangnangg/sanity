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

    uint addBus(ExpBusType type, Real load, Real generation)
    {
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

ExpPowerFlowModel ieeeCdfModel2ExpModel(const IeeeCdfModel& cdf);
