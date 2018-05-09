#pragma once
#include <memory>
#include <vector>
#include "type.hpp"

namespace sanity::powerflow
{
enum BusType
{
    GeneratorBus,
    LoadBus
};
struct LoadAttr
{
    Complex power;
};
struct GeneratorAttr
{
    Real realPower;
    Real voltageAmp;
    Real maxReact;
    Real minReact;
};
struct Bus
{
    uint idx;
    BusType type;
    union {
        LoadAttr load;
        GeneratorAttr generator;
    } attr;
    Bus(uint idx, LoadAttr load)
        : idx(idx), type(LoadBus), attr({.load = load})
    {
    }
    Bus(uint idx, GeneratorAttr gen)
        : idx(idx), type(GeneratorBus), attr({.generator = gen})
    {
    }
};

struct TransmissionLine
{
    uint idx;
    Complex totalImped;
    Real shuntSusceptance;
    uint startBus;
    uint endBus;
};

struct ShuntElement
{
    uint idx;
    uint busIdx;
    Complex impedance;
};

class PowerGrid
{
    std::vector<Bus> _buses;  // 0, 1, 2, ...
    std::vector<TransmissionLine> _lines;
    std::vector<ShuntElement> _shunts;
    uint _gen_count = 0;
    uint _load_count = 0;

public:
    uint addLoadBus(Complex power);
    uint addGeneratorBus(Real real_power, Real voltage_amp, Real min_react,
                         Real max_react);
    uint addTransmissionLine(uint start, uint end, Complex impedance,
                             Real shunt_suscep);
    uint addShuntElement(uint busIdx, Complex impedance);
    Bus& getBus(uint idx) { return _buses[idx]; }
    const Bus& getBus(uint idx) const { return _buses[idx]; }
    const TransmissionLine& getLine(uint idx) const { return _lines[idx]; }
    const ShuntElement& getShunt(uint idx) const { return _shunts[idx]; }
    uint busCount() const { return _buses.size(); }
    uint generatorCount() const { return _gen_count; }
    uint loadCount() const { return _load_count; }
    uint lineCount() const { return _lines.size(); }
    uint shuntCount() const { return _shunts.size(); }
};

}  // namespace sanity::powerflow
