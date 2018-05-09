#pragma once
#include <string>
#include <vector>
#include "model.hpp"
#include "type.hpp"
namespace sanity::powerflow
{
enum IeeeCdfBusType
{
    IeeeCdfLoad = 0,
    IeeeCdfGenPQ,
    IeeeCdfGenPV,
    IeeeCdfSlack
};
struct IeeeCdfBus
{
    int bus;                    // 1-4
    std::string busName;        // 7-17
    int area;                   // 19-20
    int lossZone;               // 21-23
    IeeeCdfBusType type;        // 25-26
    Real voltageAmp_PU;         // 28-33
    Real voltageAngle_degree;   // 34-40
    Real loadP_MW;              // 41-49
    Real loadQ_MVAR;            // 50-59
    Real genP_MW;               // 60-67
    Real genQ_MVAR;             // 68-75
    Real base_KV;               // 77-83
    Real desiredVolts_PU;       // 85-90
    Real maxQ_MVAR;             // 91-98; or voltage
    Real minQ_MVAR;             // 99-106; or voltage
    Real shuntConductanceG_PU;  // 107-114
    Real shuntSusceptanceB_PU;  // 115-122
    int remoteBusNumber;        // 124-127
};

enum IeeeCdfBranchType
{
    IeeeCdfTransLine = 0,
    IeeeCdfFixedTap,
    IeeeCdfVarTapVoltage,
    IeeeCdfVarTapMVAR,
    IeeeCdfPhaseShifter
};

struct IeeeCdfBranch
{
    int tapBus;              // 1-4
    int zBus;                // 6-9
    int area;                // 11-12
    int lossZone;            // 13-14
    int circuit;             // 17
    IeeeCdfBranchType type;  // 19
    Real resistanceR_PU;     // 20-29
    Real reactanceX_PU;      // 30-40
    Real lineCharingB_PU;    // 41-50
    int lineMVARating1;      // 51-55
    int lineMVARating2;      // 57-61
    int lineMVARating3;      // 63-67
    int controlBus;          // 69-72
    int controlBusSide;  // 74; 0: terminal, 1: near tap, 2: near impedance
    Real transFinalTurnRatio;     // 77-82
    Real phaseShifterFinalAngle;  // 84-90
    Real minTap;                  // 91-97
    Real maxTap;                  // 98-104
    Real stepSize;                // 106-111
    Real minVoltage;              // 113-119; or MVAR, MW
    Real maxVoltage;              // 120-126; or MVAR, MW
};

struct IeeeCdfModel
{
    Real MVABase;
    std::vector<IeeeCdfBus> buses;
    std::vector<IeeeCdfBranch> branches;
};

IeeeCdfModel readIeeeCdfModel(const std::string& file);

struct IeeeCdfConvertedGrid
{
    PowerGrid grid;
    uint slack;
};
IeeeCdfConvertedGrid ieeeCdf2Grid(const IeeeCdfModel& model);

}  // namespace sanity::powerflow
