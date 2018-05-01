#pragma once

#include <string>
#include <vector>
#include "model.hpp"
#include "type.hpp"

namespace sanity::powerflow
{
enum MatpowerBusType
{
    MatpowerPQBus = 1,
    MatpowerPVBus = 2,
    MatpowerRefBus = 3,
    MatpowerIsoBus = 4
};

struct MatpowerBus
{
    int idx;                     // 1
    MatpowerBusType type;        // 2
    Real loadP_MW;               // 3
    Real loadQ_MVAR;             // 4
    Real shuntConductance_MW;    // 5
    Real shuntSusceptance_MVAR;  // 6
    int areaNumber;              // 7
    Real voltageAmp_PU;          // 8
    Real voltageAngle_degree;    // 9
    Real baseV_KV;               // 10
    int zone;                    // 11
    Real maxVoltageAmp_PU;       // 12
    Real minVoltageAmp_PU;       // 13
};

struct MatpowerGenerator
{
    int busIdx;          // 1
    Real genP_MW;        // 2
    Real genQ_MVAR;      // 3
    Real maxQ_MVAR;      // 4
    Real minQ_MVAR;      // 5
    Real voltageAmp_PU;  // 6
    Real base_MVA;       // 7
    int status;          // 8
    Real maxP_MW;        // 9
    Real minP_MW;        // 10
    // version 2 part omitted
};

struct MatpowerBranch
{
    int fromBusIdx;               // 1
    int toBusIdx;                 // 2
    Real resistanceR_PU;          // 3
    Real reactanceX_PU;           // 4
    Real totalChargingSuscep_PU;  // 5
    Real rateA_MVA;               // 6
    Real rateB_MVA;               // 7
    Real rateC_MVA;               // 8
    Real ratio;                   // 9
    Real shiftAngle_degree;       // 10
    int status;                   // 11
    // version 2 part omitted
};

struct MatpowerModel
{
    Real base_MVA;
    std::vector<MatpowerBus> buses;
    std::vector<MatpowerGenerator> generators;
    std::vector<MatpowerBranch> branches;
};

MatpowerModel readMatpowerModel(const std::string& filename);

struct MatpowerConvertedGrid
{
    PowerGrid grid;
    int slack;
};
MatpowerConvertedGrid matpower2Grid(const MatpowerModel& model);

}  // namespace sanity::powerflow
