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
    int idx;
    BusType type;
    union {
        LoadAttr load;
        GeneratorAttr generator;
    } attr;
    Bus(int idx, LoadAttr load)
        : idx(idx), type(LoadBus), attr({.load = load})
    {
    }
    Bus(int idx, GeneratorAttr gen)
        : idx(idx), type(GeneratorBus), attr({.generator = gen})
    {
    }
};

struct TransmissionLine
{
    int idx;
    Complex totalImped;
    Real shuntSusceptance;
    int startBus;
    int endBus;
};

struct ShuntElement
{
    int idx;
    int busIdx;
    Complex impedance;
};

class PowerGrid
{
    std::vector<Bus> _buses;  // 0, 1, 2, ...
    std::vector<TransmissionLine> _lines;
    std::vector<ShuntElement> _shunts;
    int _gen_count = 0;
    int _load_count = 0;

public:
    int addLoadBus(Complex power);
    int addGeneratorBus(Real real_power, Real voltage_amp, Real min_react,
                        Real max_react);
    int addTransmissionLine(int start, int end, Complex impedance,
                            Real shunt_suscep);
    int addShuntElement(int busIdx, Complex impedance);
    Bus& getBus(int idx) { return _buses[(unsigned int)idx]; }
    const Bus& getBus(int idx) const { return _buses[(unsigned int)idx]; }
    const TransmissionLine& getLine(int idx) const
    {
        return _lines[(unsigned int)idx];
    }
    const ShuntElement& getShunt(int idx) const
    {
        return _shunts[(unsigned int)idx];
    }
    int busCount() const { return _buses.size(); }
    int generatorCount() const { return _gen_count; }
    int loadCount() const { return _load_count; }
    int lineCount() const { return _lines.size(); }
    int shuntCount() const { return _shunts.size(); }
};

}  // namespace sanity::powerflow
