#pragma once
#include <vector>
#include "model.hpp"
#include "type.hpp"

namespace sanity::powerflow
{
enum class DCBusType
{
    PQ,
    PV
};
struct DCBus
{
    uint idx;
    Real injectedRealPower;
    DCBusType type;
};

struct DCTransmissionLine
{
    uint idx;
    Real reactance;
    uint startBus;
    uint endBus;
};

class DCPowerFlowModel
{
    std::vector<DCBus> _buses;  // 0, 1, 2, ...
    std::vector<DCTransmissionLine> _lines;

public:
    uint addBus(DCBusType type, Real injectedRealPower)
    {
        uint idx = _buses.size();
        _buses.push_back(DCBus{.idx = idx,
                               .type = type,
                               .injectedRealPower = injectedRealPower});
        return idx;
    }
    uint addTransmissionLine(uint start, uint end, Real reactance)
    {
        uint idx = _lines.size();
        _lines.push_back({.idx = idx,
                          .reactance = reactance,
                          .startBus = start,
                          .endBus = end});
        return idx;
    }
    DCBus& getBus(uint idx) { return _buses[idx]; }
    const DCBus& getBus(uint idx) const { return _buses[idx]; }
    const DCTransmissionLine& getLine(uint idx) const { return _lines[idx]; }
    uint busCount() const { return _buses.size(); }
    uint lineCount() const { return _lines.size(); }
};

DCPowerFlowModel ACModel2DC(const PowerGrid& ac);

}  // namespace sanity::powerflow
