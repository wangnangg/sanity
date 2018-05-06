#include "dcmodel.hpp"

namespace sanity::powerflow
{
DCPowerFlowModel ACModel2DC(const PowerGrid& ac)
{
    DCPowerFlowModel dc;
    for (uint i = 0; i < (uint)ac.busCount(); i++)
    {
        const auto& bus = ac.getBus((int)i);
        if (bus.type == GeneratorBus)
        {
            dc.addBus(bus.attr.generator.realPower);
        }
        else
        {  // Load
            dc.addBus(-bus.attr.load.power.real());
        }
    }
    for (uint i = 0; i < (uint)ac.lineCount(); i++)
    {
        const auto& line = ac.getLine((int)i);
        dc.addTransmissionLine((uint)line.startBus, (uint)line.endBus,
                               line.totalImped.imag());
    }
    return dc;
}
}  // namespace sanity::powerflow
