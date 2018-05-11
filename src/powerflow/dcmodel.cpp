#include "dcmodel.hpp"

namespace sanity::powerflow
{
DCPowerFlowModel ACModel2DC(const PowerGrid& ac)
{
    DCPowerFlowModel dc;
    for (uint i = 0; i < (uint)ac.busCount(); i++)
    {
        const auto& bus = ac.getBus(i);
        if (bus.type == GeneratorBus)
        {
            dc.addBus(DCBusType::PV, bus.attr.generator.realPower);
        }
        else
        {  // Load
            dc.addBus(DCBusType::PQ, -bus.attr.load.power.real());
        }
    }
    for (uint i = 0; i < (uint)ac.lineCount(); i++)
    {
        const auto& line = ac.getLine(i);
        dc.addTransmissionLine(line.startBus, line.endBus,
                               line.totalImped.imag());
    }
    return dc;
}
}  // namespace sanity::powerflow
