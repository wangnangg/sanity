#include "ieee_cdf.hpp"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

namespace sanity::powerflow
{
static const char bus_data_head[] = "BUS DATA FOLLOWS";
static const char branch_data_head[] = "BRANCH DATA FOLLOW";

static int parseInt(const char* buf)
{
    char* next;
    int val = std::strtol(buf, &next, 10);
    if (next == buf)
    {
        std::cout << "error trying to parse int: " << buf << std::endl;
    }
    assert(next > buf);
    return val;
}

static Real parseReal(const char* buf)
{
    char* next;
    Real val = std::strtod(buf, &next);
    if (next == buf)
    {
        std::cout << "error trying to parse real: " << buf << std::endl;
    }
    assert(next > buf);
    return val;
}

/*
  struct IeeeCdfBus
  {
  int bus;0                    // 1-4
  std::string busName;6        // 7-17
  int area;18                   // 19-20
  int lossZone;20               // 21-23
  IeeeCdfBusType type;24        // 25-26
  Real voltageAmp_PU;27         // 28-33
  Real voltageAngle_degree;33   // 34-40
  Real loadP_MW;40              // 41-49
  Real loadQ_MVAR;49            // 50-59
  Real genP_MW;59               // 60-67
  Real genQ_MVAR;67             // 68-75
  Real base_KV;76               // 77-83
  Real desiredVolts_PU;84       // 85-90
  Real maxQ_MVAR;90             // 91-98; or voltage
  Real minQ_MVAR;98             // 99-106; or voltage
  Real shuntConductanceG_PU;106  // 107-114
  Real shuntSusceptanceB_PU;114  // 115-122
  int remoteBusNumber;123        // 124-127
  };
*/
static void readBusData(std::ifstream& file, std::vector<IeeeCdfBus>& buses)
{
    const int buf_len = 200;
    char buf[buf_len];
    IeeeCdfBus bus;
    while (!file.eof())
    {
        file.getline(buf, buf_len);
        bus.bus = parseInt(buf + 0);
        if (bus.bus == -999)
        {
            break;
        }
        bus.busName = std::string(buf + 6, 11);
        bus.area = parseInt(buf + 18);
        bus.lossZone = parseInt(buf + 20);
        bus.type = (IeeeCdfBusType)parseInt(buf + 24);
        bus.voltageAmp_PU = parseReal(buf + 27);
        bus.voltageAngle_degree = parseReal(buf + 33);
        bus.loadP_MW = parseReal(buf + 40);
        bus.loadQ_MVAR = parseReal(buf + 49);
        bus.genP_MW = parseReal(buf + 59);
        bus.genQ_MVAR = parseReal(buf + 67);
        bus.base_KV = parseReal(buf + 76);
        bus.desiredVolts_PU = parseReal(buf + 84);
        bus.maxQ_MVAR = parseReal(buf + 90);
        bus.minQ_MVAR = parseReal(buf + 98);
        bus.shuntConductanceG_PU = parseReal(buf + 106);
        bus.shuntSusceptanceB_PU = parseReal(buf + 114);
        bus.remoteBusNumber = parseInt(buf + 123);
        buses.push_back(bus);
    }
}
/*
struct IeeeCdfBranch
{
    int tapBus;0              // 1-4
    int zBus;5                // 6-9
    int area;10                // 11-12
    int lossZone;12            // 13-14
    int circuit;16             // 17
    IeeeCdfBranchType type;18  // 19
    Real resistanceR_PU;19     // 20-29
    Real reactanceX_PU;29      // 30-40
    Real lineCharingB_PU;40    // 41-50
    int lineMVARating1;50      // 51-55
    int lineMVARating2;56      // 57-61
    int lineMVARating3;62      // 63-67
    int controlBus;68          // 69-72
    int controlBusSide;73  // 74; 0: terminal, 1: near tap, 2: near impedance
    Real transFinalTurnRatio;76 //77-82
    Real phaseShifterFinalAngle;83 //84-90
    Real minTap;90 //91-97
    Real maxTap;97 //98-104
    Real stepSize;105 //106-111
    Real minVoltage;112 //113-119; or MVAR, MW
    Real maxVoltage;119 //120-126; or MVAR, MW
};
*/
static void readBranchData(std::ifstream& file,
                           std::vector<IeeeCdfBranch>& branches)
{
    const int buf_len = 200;
    char buf[buf_len];
    IeeeCdfBranch branch;
    while (!file.eof())
    {
        file.getline(buf, buf_len);
        branch.tapBus = parseInt(buf + 0);
        if (branch.tapBus == -999)
        {
            break;
        }
        branch.zBus = parseInt(buf + 5);
        branch.area = parseInt(buf + 10);
        branch.lossZone = parseInt(buf + 12);
        branch.circuit = parseInt(buf + 16);
        branch.type = (IeeeCdfBranchType)parseInt(buf + 18);
        branch.resistanceR_PU = parseReal(buf + 19);
        branch.reactanceX_PU = parseReal(buf + 29);
        branch.lineCharingB_PU = parseReal(buf + 40);
        branch.lineMVARating1 = parseInt(buf + 50);
        branch.lineMVARating2 = parseInt(buf + 56);
        branch.lineMVARating3 = parseInt(buf + 62);
        branch.controlBus = parseInt(buf + 68);
        branch.controlBusSide = parseInt(buf + 73);
        branch.transFinalTurnRatio = parseReal(buf + 76);
        branch.phaseShifterFinalAngle = parseReal(buf + 83);
        branch.minTap = parseReal(buf + 90);
        branch.maxTap = parseReal(buf + 97);
        branch.stepSize = parseReal(buf + 105);
        branch.minVoltage = parseReal(buf + 112);
        branch.maxVoltage = parseReal(buf + 119);
        branches.push_back(branch);
    }
}

IeeeCdfModel readIeeeCdfModel(const std::string& filename)
{
    IeeeCdfModel model;
    std::ifstream file;
    const int buf_len = 200;
    char buf[buf_len];
    file.open(filename);
    if (file.fail())
    {
        throw std::ios_base::failure("failed to open: " + filename);
    }
    if (!file.eof())
    {
        file.getline(buf, buf_len);
        model.MVABase = parseReal(buf + 31);
    }
    while (!file.eof())
    {
        file.getline(buf, buf_len);
        if (std::strncmp(buf, bus_data_head, std::strlen(bus_data_head)) == 0)
        {
            readBusData(file, model.buses);
        }
        else if (std::strncmp(buf, branch_data_head,
                              std::strlen(branch_data_head)) == 0)
        {
            readBranchData(file, model.branches);
        }
    }
    file.close();
    return model;
}

IeeeCdfConvertedGrid ieeeCdf2Grid(const IeeeCdfModel& model)
{
    PowerGrid grid;
    int slack = -1;
    for (const auto& bus : model.buses)
    {
        int bus_id = -1;
        switch (bus.type)
        {
            case IeeeCdfLoad:
                bus_id = (int)grid.addLoadBus(
                    Complex(bus.loadP_MW, bus.loadQ_MVAR) / model.MVABase);
                break;
            case IeeeCdfGenPV:
                bus_id = (int)grid.addGeneratorBus(
                    (bus.genP_MW - bus.loadP_MW) / model.MVABase,
                    bus.desiredVolts_PU,
                    (bus.minQ_MVAR - bus.loadQ_MVAR) / model.MVABase,
                    (bus.maxQ_MVAR - bus.loadQ_MVAR) / model.MVABase);
                break;
            case IeeeCdfSlack:
                bus_id = (int)grid.addGeneratorBus(
                    (bus.genP_MW - bus.loadP_MW) / model.MVABase,
                    bus.desiredVolts_PU,
                    (bus.minQ_MVAR - bus.loadQ_MVAR) / model.MVABase,
                    (bus.maxQ_MVAR - bus.loadQ_MVAR) / model.MVABase);
                slack = bus_id;
                break;
            case IeeeCdfGenPQ:
            default:  // unsupported
                assert(false);
                break;
        }
        assert(bus_id >= 0);
        grid.addShuntElement((uint)bus_id,
                             1.0 / Complex(bus.shuntConductanceG_PU,
                                           bus.shuntSusceptanceB_PU));
    }
    for (const auto& branch : model.branches)
    {
        switch (branch.type)
        {
            case IeeeCdfTransLine:
            case IeeeCdfFixedTap:
                grid.addTransmissionLine(
                    (uint)(branch.tapBus - 1), (uint)(branch.zBus - 1),
                    Complex(branch.resistanceR_PU, branch.reactanceX_PU),
                    branch.lineCharingB_PU / 2);
                break;
            default:  // unsupported
                std::cout << "unsupported line type: " << branch.type
                          << std::endl;
                assert(false);
                break;
        }
    }
    assert(slack >= 0);
    return IeeeCdfConvertedGrid{.grid = grid, .slack = (uint)slack};
}
}  // namespace sanity::powerflow
