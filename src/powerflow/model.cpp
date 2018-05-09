#include "model.hpp"

namespace sanity::powerflow
{
uint PowerGrid::addLoadBus(Complex power)
{
    uint idx = _buses.size();
    Bus bus(idx, LoadAttr{power});
    _buses.push_back(bus);
    _load_count += 1;
    return idx;
}

uint PowerGrid::addGeneratorBus(Real real_power, Real voltage_amp,
                                Real min_react, Real max_react)
{
    uint idx = _buses.size();
    Bus bus(idx, GeneratorAttr{.realPower = real_power,
                               .voltageAmp = voltage_amp,
                               .minReact = min_react,
                               .maxReact = max_react});
    _buses.push_back(bus);
    _gen_count += 1;
    return idx;
}

uint PowerGrid::addTransmissionLine(uint start, uint end, Complex impedance,
                                    Real shunt_suscep)
{
    uint idx = _lines.size();
    auto line = TransmissionLine{.idx = idx,
                                 .totalImped = impedance,
                                 .shuntSusceptance = shunt_suscep,
                                 .startBus = start,
                                 .endBus = end};
    _lines.push_back(line);
    return idx;
}

uint PowerGrid::addShuntElement(uint busIdx, Complex impedance)
{
    uint idx = _shunts.size();
    auto shunt =
        ShuntElement{.idx = idx, .busIdx = busIdx, .impedance = impedance};
    _shunts.push_back(shunt);
    return idx;
}

}  // namespace sanity::powerflow
