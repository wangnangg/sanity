#include "model.hpp"

namespace sanity::powerflow
{
int PowerGrid::addLoadBus(Complex power)
{
    int idx = _buses.size();
    Bus bus(idx, LoadAttr{power});
    _buses.push_back(bus);
    _load_count += 1;
    return idx;
}

int PowerGrid::addGeneratorBus(Real real_power, Real voltage_amp,
                               Real min_react, Real max_react)
{
    int idx = _buses.size();
    Bus bus(idx, GeneratorAttr{.realPower = real_power,
                               .voltageAmp = voltage_amp,
                               .minReact = min_react,
                               .maxReact = max_react});
    _buses.push_back(bus);
    _gen_count += 1;
    return idx;
}

int PowerGrid::addTransmissionLine(int start, int end, Complex impedance,
                                   Real shunt_suscep)
{
    int idx = _lines.size();
    auto line = TransmissionLine{.idx = idx,
                                 .totalImped = impedance,
                                 .shuntSusceptance = shunt_suscep,
                                 .startBus = start,
                                 .endBus = end};
    _lines.push_back(line);
    return idx;
}

int PowerGrid::addShuntElement(int busIdx, Complex impedance)
{
    int idx = _shunts.size();
    auto shunt =
        ShuntElement{.idx = idx, .busIdx = busIdx, .impedance = impedance};
    _shunts.push_back(shunt);
    return idx;
}

}  // namespace sanity::powerflow
