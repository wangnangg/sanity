#include "matpower_format.hpp"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>

namespace sanity::powerflow
{
static int parseInt(char** buf)
{
    char* curr = *buf;
    int val = std::strtol(curr, buf, 10);
    if (*buf == curr)
    {
        std::cout << "error trying to parse int: " << buf << std::endl;
    }
    assert(*buf > curr);
    return val;
}

static Real parseReal(char** buf)
{
    char* curr = *buf;
    Real val = std::strtod(curr, buf);
    if (*buf == curr)
    {
        std::cout << "error trying to parse real: " << buf << std::endl;
    }
    assert(*buf > curr);
    return val;
}

// struct MatpowerBus
// {
//     int idx; //1
//     MatpowerBusType type; //2
//     Real loadP_MW; //3
//     Real loadQ_MVAR; //4
//     Real shuntConductance_MW; //5
//     Real shuntSusceptance_MVAR; //6
//     int areaNumber; //7
//     Real voltageAmp_PU; //8
//     Real voltageAngle_degree; //9
//     Real baseV_KV; //10
//     int zone; //11
//     Real maxVoltageAmp_PU; //12
//     Real minVoltageAmp_PU; //13
// };
static std::vector<MatpowerBus> readBusData(std::ifstream& file, int count)
{
    const int buf_len = 1000;
    char buf[buf_len];
    std::vector<MatpowerBus> buses;
    for (int i = 0; i < count; i++)
    {
        assert(!file.eof());
        file.getline(buf, buf_len);
        char* next = buf;
        MatpowerBus bus;
        bus.idx = parseInt(&next);
        bus.type = (MatpowerBusType)parseInt(&next);
        bus.loadP_MW = parseReal(&next);
        bus.loadQ_MVAR = parseReal(&next);
        bus.shuntConductance_MW = parseReal(&next);
        bus.shuntSusceptance_MVAR = parseReal(&next);
        bus.areaNumber = parseInt(&next);
        bus.voltageAmp_PU = parseReal(&next);
        bus.voltageAngle_degree = parseReal(&next);
        bus.baseV_KV = parseReal(&next);
        bus.zone = parseInt(&next);
        bus.maxVoltageAmp_PU = parseReal(&next);
        bus.minVoltageAmp_PU = parseReal(&next);
        buses.push_back(bus);
    }
    return buses;
}

// struct MatpowerGenerator
// {
//     int busIdx; //1
//     Real genP_MW; //2
//     Real genQ_MVAR; //3
//     Real maxQ_MVAR; //4
//     Real minQ_MVAR; //5
//     Real voltageAmp_PU; //6
//     Real base_MVA; //7
//     int status; //8
//     Real maxP_MW; //9
//     Real minP_MW; //10
//     //version 2 part omitted
// };
static std::vector<MatpowerGenerator> readGeneratorData(std::ifstream& file,
                                                        int count)
{
    const int buf_len = 1000;
    char buf[buf_len];
    std::vector<MatpowerGenerator> gens;
    for (int i = 0; i < count; i++)
    {
        assert(!file.eof());
        file.getline(buf, buf_len);
        char* next = buf;
        MatpowerGenerator gen;
        gen.busIdx = parseInt(&next);
        gen.genP_MW = parseReal(&next);
        gen.genQ_MVAR = parseReal(&next);
        gen.maxQ_MVAR = parseReal(&next);
        gen.minQ_MVAR = parseReal(&next);
        gen.voltageAmp_PU = parseReal(&next);
        gen.base_MVA = parseReal(&next);
        gen.status = parseInt(&next);
        gen.maxP_MW = parseReal(&next);
        gen.minP_MW = parseReal(&next);
        gens.push_back(gen);
    }
    return gens;
}

// struct MatpowerBranch
// {
//     int fromBusIdx; //1
//     int toBusIdx; //2
//     Real resistanceR_PU; //3
//     Real reactanceX_PU; //4
//     Real totalChargingSuscep_PU; //5
//     Real rateA_MVA; //6
//     Real rateB_MVA; //7
//     Real rateC_MVA; //8
//     Real ratio; //9
//     Real shiftAngle_degree; //10
//     int status;  //11
//     //version 2 part omitted
// };
static std::vector<MatpowerBranch> readBranchData(std::ifstream& file,
                                                  int count)
{
    const int buf_len = 1000;
    char buf[buf_len];
    std::vector<MatpowerBranch> branches;
    for (int i = 0; i < count; i++)
    {
        assert(!file.eof());
        file.getline(buf, buf_len);
        char* next = buf;
        MatpowerBranch br;
        br.fromBusIdx = parseInt(&next);
        br.toBusIdx = parseInt(&next);
        br.resistanceR_PU = parseReal(&next);
        br.reactanceX_PU = parseReal(&next);
        br.totalChargingSuscep_PU = parseReal(&next);
        br.rateA_MVA = parseReal(&next);
        br.rateB_MVA = parseReal(&next);
        br.rateC_MVA = parseReal(&next);
        br.ratio = parseReal(&next);
        br.shiftAngle_degree = parseReal(&next);
        br.status = parseInt(&next);
        branches.push_back(br);
    }
    return branches;
}

MatpowerModel readMatpowerModel(const std::string& filename)
{
    MatpowerModel model;
    std::ifstream file;
    const int buf_len = 1000;
    char buf[buf_len];
    file.open(filename);
    if (file.fail())
    {
        throw std::ios_base::failure("failed to open: " + filename);
    }

    file.getline(buf, buf_len);
    assert(!file.eof());
    char* next = buf;
    model.base_MVA = parseReal(&next);

    file.getline(buf, buf_len);
    assert(!file.eof());
    next = buf;
    int busCount = parseInt(&next);
    model.buses = readBusData(file, busCount);

    file.getline(buf, buf_len);
    assert(!file.eof());
    next = buf;
    int genCount = parseInt(&next);
    model.generators = readGeneratorData(file, genCount);

    file.getline(buf, buf_len);
    assert(!file.eof());
    next = buf;
    int branchCount = parseInt(&next);
    model.branches = readBranchData(file, branchCount);

    return model;
}

MatpowerConvertedGrid matpower2Grid(const MatpowerModel& model)
{
    PowerGrid grid;
    int slackId = -1;
    std::map<int, std::vector<MatpowerGenerator>> bus2gens;
    for (const auto& g : model.generators)
    {
        bus2gens[g.busIdx].push_back(g);
    }
    std::map<uint, uint> matId2gridId;
    for (const auto& b : model.buses)
    {
        int gridId = -1;
        switch (b.type)
        {
            case MatpowerPQBus:  // load only
            {
                assert(bus2gens[b.idx].size() == 0);
                gridId = (int)grid.addLoadBus(
                    Complex(b.loadP_MW, b.loadQ_MVAR) / model.base_MVA);
            }
            break;
            case MatpowerPVBus:  // generators and loads
            {
                assert(bus2gens[b.idx].size() > 0);
                Real genP_MW = 0.0;
                Real voltageAmp_PU = 0.0;
                for (const auto& g : bus2gens[b.idx])
                {
                    genP_MW += g.genP_MW;
                    voltageAmp_PU = g.voltageAmp_PU;
                }
                assert(voltageAmp_PU == b.voltageAmp_PU);
                gridId = (int)grid.addGeneratorBus(
                    (genP_MW - b.loadP_MW) / model.base_MVA, voltageAmp_PU, 0,
                    0);
            }
            break;
            case MatpowerRefBus:
            {
                Real genP_MW = 0.0;
                Real voltageAmp_PU = 0.0;
                for (const auto& g : bus2gens[b.idx])
                {
                    genP_MW += g.genP_MW;
                    voltageAmp_PU = g.voltageAmp_PU;
                }
                gridId = (int)grid.addGeneratorBus(
                    (genP_MW - b.loadP_MW) / model.base_MVA, voltageAmp_PU, 0,
                    0);
                assert(voltageAmp_PU == b.voltageAmp_PU);
                slackId = gridId;
            }
            break;
            default:
                std::cout << "unsupported bus type:" << MatpowerIsoBus
                          << std::endl;
                assert(false);
                break;
        }
        assert(gridId >= 0);
        grid.addShuntElement(
            (uint)gridId,
            1.0 / (Complex(b.shuntConductance_MW, b.shuntSusceptance_MVAR) /
                   model.base_MVA));
        matId2gridId[(uint)b.idx] = (uint)gridId;
    }
    for (const auto& br : model.branches)
    {
        uint fromBus = matId2gridId.at((uint)br.fromBusIdx);
        uint toBus = matId2gridId.at((uint)br.toBusIdx);
        grid.addTransmissionLine(fromBus, toBus,
                                 Complex(br.resistanceR_PU, br.reactanceX_PU),
                                 br.totalChargingSuscep_PU / 2.0);
    }
    return {grid, (uint)slackId};
}

}  // namespace sanity::powerflow
