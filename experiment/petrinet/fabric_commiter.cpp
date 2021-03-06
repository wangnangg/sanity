#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include "gtest/gtest.h"
#include "linear.hpp"
#include "petrinet.hpp"
#include "simulate.hpp"
#include "splinear.hpp"
#include "timer.hpp"

using namespace sanity::petrinet;
using namespace sanity::linear;
using namespace sanity::splinear;

std::tuple<SrnCreator, std::unordered_map<std::string, uint>,
           std::unordered_map<std::string, uint>>
commiterModel(Real block_arr_rate, uint block_size, Real validate_rate,
              uint vscc_th, Real mvcc_rate, Real ledger_rate)
{
    SrnCreator ct;
    std::unordered_map<std::string, uint> n2i;
    std::unordered_map<std::string, uint> tn2i;

    auto p_vscc = ct.place();
    n2i["p_vscc"] = p_vscc;

    auto p_vscc_end = ct.place();
    n2i["p_vscc_end"] = p_vscc_end;

    auto p_mvcc = ct.place();
    n2i["p_mvcc"] = p_mvcc;

    auto p_ledger = ct.place();
    n2i["p_ledger"] = p_ledger;

    uint t_block_arr = ct.expTrans(block_arr_rate)
                           .oarc(p_vscc, block_size)
                           .harc(p_vscc, block_size * 2)
                           .idx();
    tn2i["t_block_arr"] = t_block_arr;

    uint t_vscc = ct.expTrans([=](PetriNetState st) {
                        uint npar =
                            std::min(st.marking->nToken(p_vscc), vscc_th);
                        return (Real)npar * validate_rate;
                    })
                      .iarc(p_vscc)
                      .harc(p_vscc_end, block_size * 2)
                      .oarc(p_vscc_end)
                      .idx();
    tn2i["t_vscc"] = t_vscc;

    uint t_block_pack = ct.immTrans()
                            .iarc(p_vscc_end, block_size)
                            .harc(p_mvcc, 2)
                            .oarc(p_mvcc)
                            .idx();

    uint t_mvcc = ct.expTrans(mvcc_rate)
                      .iarc(p_mvcc)
                      .oarc(p_ledger)
                      .harc(p_ledger, 2)
                      .idx();
    tn2i["t_mvcc"] = t_mvcc;

    uint t_ledger = ct.expTrans(ledger_rate).iarc(p_ledger).idx();
    tn2i["t_ledger"] = t_ledger;

    return std::make_tuple(std::move(ct), n2i, tn2i);
}

extern char** global_argv;

TEST(fabric, commiter)
{
    uint block_size = (uint)std::stoi(global_argv[1]);
    for (Real block_arr_rate = 1; block_arr_rate <= 10; block_arr_rate += 1)
    {
        std::cout << "block arrival rate: " << block_arr_rate << std::endl;
        std::cout << "block size: " << block_size << std::endl;
        std::cout << "tx arrival rate: " << block_arr_rate * block_size
                  << std::endl;

        std::map<uint, Real> cons_rate = {
            {40, 75.74}, {80, 81.6}, {120, 93.56}};
        std::map<uint, Real> mvcc_rate = {
            {40, 2.559}, {80, 5.1}, {120, 7.202}};
        std::map<uint, Real> ledger_rate = {
            {40, 207.8}, {80, 208.3}, {120, 188.4}};
        uint vscc_th = 4;
        auto [ct, n2i, tn2i] =
            commiterModel(block_arr_rate,                   // block_arr_rate
                          block_size,                       // block_size
                          1e3 / 2.524,                      // validate_rate
                          vscc_th,                          // vscc_th
                          1e3 / mvcc_rate.at(block_size),   // mvcc_rate
                          1e3 / ledger_rate.at(block_size)  // ledger_rate
            );
        uint t_ledger = tn2i["t_ledger"];
        auto srn = ct.create();
        std::vector<std::string> i2n(srn.placeCount());
        for (const auto& pair : n2i)
        {
            i2n[pair.second] = pair.first;
        }
        auto init_mk = ct.marking();

        auto rg = genReducedReachGraph(srn, init_mk);
        std::cout << "marking count: " << rg.nodeMarkings.size() << std::endl;
        auto w = 1.0;
        auto tol = 1e-6;
        uint max_iter = 1000;

        SrnSteadyStateSol sol =
            srnSteadyStateSor(rg.graph, rg.edgeRates, w, tol, max_iter);

        auto vscc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_vscc"]));
        auto mvcc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_mvcc"]));
        auto ledger_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                         srnPlaceTokenFunc(n2i["p_ledger"]));

        auto p_vscc = n2i.at("p_vscc");
        auto vscc_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [vscc_th, p_vscc](PetriNetState st) {
                uint ntoken = st.marking->nToken(p_vscc);
                if (ntoken <= vscc_th)
                {
                    return (Real)ntoken / (Real)(vscc_th);
                }
                else
                {
                    return 1.0;
                }
            });

        auto t_mvcc = tn2i.at("t_mvcc");
        auto mvcc_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_mvcc](PetriNetState st) {
                return st.net->isTransitionEnabled(t_mvcc, st.marking) ? 1.0
                                                                       : 0.0;
            });

        auto ledger_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_ledger](PetriNetState st) {
                return st.net->isTransitionEnabled(t_ledger, st.marking)
                           ? 1.0
                           : 0.0;
            });

        std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
        std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
        std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
        std::cout << "vscc_util: " << vscc_util << std::endl;
        std::cout << "mvcc_util: " << mvcc_util << std::endl;
        std::cout << "ledger_util: " << ledger_util << std::endl;
        std::cout << std::endl;
    }
}

auto times =
    std::vector<Real>{0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.1, 0.2, 0.3, 0.4,
                      0.5, 1.0,  2.0,  4.0,  6,    8,    10,  15,  20};
Real block_arr_rate = 3;
uint block_size = 80;
std::map<uint, Real> cons_rate = {{40, 75.74}, {80, 81.6}, {120, 93.56}};
std::map<uint, Real> mvcc_rate = {{40, 2.559}, {80, 5.1}, {120, 7.202}};
std::map<uint, Real> ledger_rate = {{40, 207.8}, {80, 208.3}, {120, 188.4}};
uint vscc_th = 4;

Real start_high_rate = 0.125;
Real stop_high_rate = 0.5;
Real high_rate = 2.0 * 3.0;
Real low_rate = 0.75 * 3.0;

TEST(fabric, commiter_ts)
{
    std::cout << "block arrival rate: " << block_arr_rate << std::endl;
    std::cout << "block size: " << block_size << std::endl;
    std::cout << "tx arrival rate: " << block_arr_rate * block_size
              << std::endl;
    for (Real time : times)
    {
        std::cout << "time: " << time << std::endl;

        auto [ct, n2i, tn2i] =
            commiterModel(block_arr_rate,                   // block_arr_rate
                          block_size,                       // block_size
                          1e3 / 2.524,                      // validate_rate
                          vscc_th,                          // vscc_th
                          1e3 / mvcc_rate.at(block_size),   // mvcc_rate
                          1e3 / ledger_rate.at(block_size)  // ledger_rate
            );
        uint t_ledger = tn2i["t_ledger"];
        auto srn = ct.create();
        std::vector<std::string> i2n(srn.placeCount());
        for (const auto& pair : n2i)
        {
            i2n[pair.second] = pair.first;
        }
        auto init_mk = ct.marking();

        auto rg = genReducedReachGraph(srn, init_mk);
        std::cout << "marking count: " << rg.nodeMarkings.size() << std::endl;

        auto sol =
            srnTransientProb(rg.graph, rg.edgeRates, rg.initProbs, time);

        auto vscc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_vscc"]));
        auto mvcc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_mvcc"]));
        auto ledger_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                         srnPlaceTokenFunc(n2i["p_ledger"]));

        auto p_vscc = n2i.at("p_vscc");
        auto vscc_util =
            srnProbReward(srn, sol, rg.nodeMarkings, [=](PetriNetState st) {
                uint ntoken = st.marking->nToken(p_vscc);
                if (ntoken <= vscc_th)
                {
                    return (Real)ntoken / (Real)(vscc_th);
                }
                else
                {
                    return 1.0;
                }
            });

        auto t_mvcc = tn2i.at("t_mvcc");
        auto mvcc_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_mvcc](PetriNetState st) {
                return st.net->isTransitionEnabled(t_mvcc, st.marking) ? 1.0
                                                                       : 0.0;
            });

        auto ledger_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_ledger](PetriNetState st) {
                return st.net->isTransitionEnabled(t_ledger, st.marking)
                           ? 1.0
                           : 0.0;
            });

        std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
        std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
        std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
        std::cout << "vscc_util: " << vscc_util << std::endl;
        std::cout << "mvcc_util: " << mvcc_util << std::endl;
        std::cout << "ledger_util: " << ledger_util << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "steady state: " << std::endl;

        auto [ct, n2i, tn2i] =
            commiterModel(block_arr_rate,                   // block_arr_rate
                          block_size,                       // block_size
                          1e3 / 2.524,                      // validate_rate
                          vscc_th,                          // vscc_th
                          1e3 / mvcc_rate.at(block_size),   // mvcc_rate
                          1e3 / ledger_rate.at(block_size)  // ledger_rate
            );
        uint t_ledger = tn2i["t_ledger"];
        auto srn = ct.create();
        std::vector<std::string> i2n(srn.placeCount());
        for (const auto& pair : n2i)
        {
            i2n[pair.second] = pair.first;
        }
        auto init_mk = ct.marking();

        auto rg = genReducedReachGraph(srn, init_mk);
        std::cout << "marking count: " << rg.nodeMarkings.size() << std::endl;

        auto w = 1.0;
        auto tol = 1e-6;
        uint max_iter = 1000;

        SrnSteadyStateSol sol =
            srnSteadyStateSor(rg.graph, rg.edgeRates, w, tol, max_iter);

        auto vscc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_vscc"]));
        auto mvcc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_mvcc"]));
        auto ledger_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                         srnPlaceTokenFunc(n2i["p_ledger"]));

        auto p_vscc = n2i.at("p_vscc");
        auto vscc_util =
            srnProbReward(srn, sol, rg.nodeMarkings, [=](PetriNetState st) {
                uint ntoken = st.marking->nToken(p_vscc);
                if (ntoken <= vscc_th)
                {
                    return (Real)ntoken / (Real)(vscc_th);
                }
                else
                {
                    return 1.0;
                }
            });

        auto t_mvcc = tn2i.at("t_mvcc");
        auto mvcc_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_mvcc](PetriNetState st) {
                return st.net->isTransitionEnabled(t_mvcc, st.marking) ? 1.0
                                                                       : 0.0;
            });

        auto ledger_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_ledger](PetriNetState st) {
                return st.net->isTransitionEnabled(t_ledger, st.marking)
                           ? 1.0
                           : 0.0;
            });

        std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
        std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
        std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
        std::cout << "vscc_util: " << vscc_util << std::endl;
        std::cout << "mvcc_util: " << mvcc_util << std::endl;
        std::cout << "ledger_util: " << ledger_util << std::endl;
        std::cout << std::endl;
    }
}

std::tuple<SrnCreator, std::unordered_map<std::string, uint>,
           std::unordered_map<std::string, uint>>
commiterMMPPModel(Real start_high_rate, Real stop_high_rate, Real high_rate,
                  Real low_rate, uint block_size, Real validate_rate,
                  uint vscc_th, Real mvcc_rate, Real ledger_rate)
{
    SrnCreator ct;
    std::unordered_map<std::string, uint> n2i;
    std::unordered_map<std::string, uint> tn2i;

    auto p_mmpp = ct.place(1);
    n2i["p_mmpp"] = p_mmpp;

    auto p_vscc = ct.place();
    n2i["p_vscc"] = p_vscc;

    auto p_vscc_end = ct.place();
    n2i["p_vscc_end"] = p_vscc_end;

    auto p_mvcc = ct.place();
    n2i["p_mvcc"] = p_mvcc;

    auto p_ledger = ct.place();
    n2i["p_ledger"] = p_ledger;

    uint t_start =
        ct.expTrans(start_high_rate).oarc(p_mmpp).harc(p_mmpp).idx();
    uint t_stop = ct.expTrans(stop_high_rate).iarc(p_mmpp).idx();
    tn2i["t_start"] = t_start;
    tn2i["t_stop"] = t_stop;

    uint t_block_arr =
        ct.expTrans([p_mmpp, high_rate, low_rate](PetriNetState st) {
              if (st.marking->nToken(p_mmpp) == 1)
              {
                  return high_rate;
              }
              else
              {
                  return low_rate;
              }
          })
            .oarc(p_vscc, block_size)
            .harc(p_vscc, block_size * 2)
            .idx();
    tn2i["t_block_arr"] = t_block_arr;

    uint t_vscc = ct.expTrans([=](PetriNetState st) {
                        uint npar =
                            std::min(st.marking->nToken(p_vscc), vscc_th);
                        return (Real)npar * validate_rate;
                    })
                      .iarc(p_vscc)
                      .harc(p_vscc_end, block_size * 2)
                      .oarc(p_vscc_end)
                      .idx();
    tn2i["t_vscc"] = t_vscc;

    uint t_block_pack = ct.immTrans()
                            .iarc(p_vscc_end, block_size)
                            .harc(p_mvcc, 2)
                            .oarc(p_mvcc)
                            .idx();

    uint t_mvcc = ct.expTrans(mvcc_rate)
                      .iarc(p_mvcc)
                      .oarc(p_ledger)
                      .harc(p_ledger, 2)
                      .idx();
    tn2i["t_mvcc"] = t_mvcc;

    uint t_ledger = ct.expTrans(ledger_rate).iarc(p_ledger).idx();
    tn2i["t_ledger"] = t_ledger;

    return std::make_tuple(std::move(ct), n2i, tn2i);
}

TEST(fabric, commiter_ts_mmpp)
{
    Real avg_block_arr_rate =
        ((1.0 / stop_high_rate) * high_rate +
         (1.0 / start_high_rate) * low_rate) /
        ((1.0 / stop_high_rate) + (1.0 / start_high_rate));
    std::cout << "avg block arrival rate: " << avg_block_arr_rate
              << std::endl;
    std::cout << "block size: " << block_size << std::endl;
    std::cout << "tx arrival rate: " << block_arr_rate * block_size
              << std::endl;
    for (Real time : times)
    {
        std::cout << "time: " << time << std::endl;
        auto [ct, n2i, tn2i] =
            commiterMMPPModel(start_high_rate,  // start_high_rate
                              stop_high_rate,   // stop_high_rate
                              high_rate,        // high_rate
                              low_rate,         // low_rate
                              block_size,       // block_size
                              1e3 / 2.524,      // validate_rate
                              vscc_th,          // vscc_th
                              1e3 / mvcc_rate.at(block_size),   // mvcc_rate
                              1e3 / ledger_rate.at(block_size)  // ledger_rate
            );
        uint t_ledger = tn2i["t_ledger"];
        auto srn = ct.create();
        std::vector<std::string> i2n(srn.placeCount());
        for (const auto& pair : n2i)
        {
            i2n[pair.second] = pair.first;
        }
        auto init_mk = ct.marking();

        auto rg = genReducedReachGraph(srn, init_mk);
        std::cout << "marking count: " << rg.nodeMarkings.size() << std::endl;

        auto sol =
            srnTransientProb(rg.graph, rg.edgeRates, rg.initProbs, time);

        auto vscc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_vscc"]));
        auto mvcc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_mvcc"]));
        auto ledger_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                         srnPlaceTokenFunc(n2i["p_ledger"]));

        auto p_vscc = n2i.at("p_vscc");
        auto vscc_util =
            srnProbReward(srn, sol, rg.nodeMarkings, [=](PetriNetState st) {
                uint ntoken = st.marking->nToken(p_vscc);
                if (ntoken <= vscc_th)
                {
                    return (Real)ntoken / (Real)(vscc_th);
                }
                else
                {
                    return 1.0;
                }
            });

        auto t_mvcc = tn2i.at("t_mvcc");
        auto mvcc_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_mvcc](PetriNetState st) {
                return st.net->isTransitionEnabled(t_mvcc, st.marking) ? 1.0
                                                                       : 0.0;
            });

        auto ledger_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_ledger](PetriNetState st) {
                return st.net->isTransitionEnabled(t_ledger, st.marking)
                           ? 1.0
                           : 0.0;
            });

        std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
        std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
        std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
        std::cout << "vscc_util: " << vscc_util << std::endl;
        std::cout << "mvcc_util: " << mvcc_util << std::endl;
        std::cout << "ledger_util: " << ledger_util << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "steady state: " << std::endl;

        auto [ct, n2i, tn2i] =
            commiterMMPPModel(start_high_rate,  // start_high_rate
                              stop_high_rate,   // stop_high_rate
                              high_rate,        // high_rate
                              low_rate,         // low_rate
                              block_size,       // block_size
                              1e3 / 2.524,      // validate_rate
                              vscc_th,          // vscc_th
                              1e3 / mvcc_rate.at(block_size),   // mvcc_rate
                              1e3 / ledger_rate.at(block_size)  // ledger_rate
            );
        uint t_ledger = tn2i["t_ledger"];
        auto srn = ct.create();
        std::vector<std::string> i2n(srn.placeCount());
        for (const auto& pair : n2i)
        {
            i2n[pair.second] = pair.first;
        }
        auto init_mk = ct.marking();

        auto rg = genReducedReachGraph(srn, init_mk);
        std::cout << "marking count: " << rg.nodeMarkings.size() << std::endl;

        auto w = 1.0;
        auto tol = 1e-6;
        uint max_iter = 1000;

        SrnSteadyStateSol sol =
            srnSteadyStateSor(rg.graph, rg.edgeRates, w, tol, max_iter);

        auto mean_arr_rate =
            srnProbReward(srn, sol, rg.nodeMarkings,
                          srnTransRateFunc(tn2i.at("t_block_arr")));

        auto high_prob = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i.at("p_mmpp")));
        std::cout << "mean_arr_rate: " << mean_arr_rate << std::endl;
        std::cout << "high_prob: " << high_prob << std::endl;

        auto vscc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_vscc"]));
        auto mvcc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                       srnPlaceTokenFunc(n2i["p_mvcc"]));
        auto ledger_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                         srnPlaceTokenFunc(n2i["p_ledger"]));

        auto p_vscc = n2i.at("p_vscc");
        auto vscc_util =
            srnProbReward(srn, sol, rg.nodeMarkings, [=](PetriNetState st) {
                uint ntoken = st.marking->nToken(p_vscc);
                if (ntoken <= vscc_th)
                {
                    return (Real)ntoken / (Real)(vscc_th);
                }
                else
                {
                    return 1.0;
                }
            });

        auto t_mvcc = tn2i.at("t_mvcc");
        auto mvcc_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_mvcc](PetriNetState st) {
                return st.net->isTransitionEnabled(t_mvcc, st.marking) ? 1.0
                                                                       : 0.0;
            });

        auto ledger_util = srnProbReward(
            srn, sol, rg.nodeMarkings, [t_ledger](PetriNetState st) {
                return st.net->isTransitionEnabled(t_ledger, st.marking)
                           ? 1.0
                           : 0.0;
            });

        std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
        std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
        std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
        std::cout << "vscc_util: " << vscc_util << std::endl;
        std::cout << "mvcc_util: " << mvcc_util << std::endl;
        std::cout << "ledger_util: " << ledger_util << std::endl;
        std::cout << std::endl;
    }
}

TEST(fabric, commiter_cmp)
{
    uint block_size = (uint)std::stoi(global_argv[1]);
    uint vscc_th = (uint)std::stoi(global_argv[2]);
    Real block_arr_rate = 3.0;
    std::cout << "block arrival rate: " << block_arr_rate << std::endl;
    std::cout << "block size: " << block_size << std::endl;
    std::cout << "cores: " << vscc_th << std::endl;
    std::cout << "tx arrival rate: " << block_arr_rate * block_size
              << std::endl;

    std::map<uint, Real> cons_rate = {{40, 75.74}, {80, 81.6}, {120, 93.56}};
    std::map<uint, Real> mvcc_rate = {{40, 2.559}, {80, 5.1}, {120, 7.202}};
    std::map<uint, Real> ledger_rate = {
        {40, 207.8}, {80, 208.3}, {120, 188.4}};
    auto [ct, n2i, tn2i] =
        commiterModel(block_arr_rate,                   // block_arr_rate
                      block_size,                       // block_size
                      1e3 / 2.524,                      // validate_rate
                      vscc_th,                          // vscc_th
                      1e3 / mvcc_rate.at(block_size),   // mvcc_rate
                      1e3 / ledger_rate.at(block_size)  // ledger_rate
        );
    uint t_ledger = tn2i["t_ledger"];
    auto srn = ct.create();
    std::vector<std::string> i2n(srn.placeCount());
    for (const auto& pair : n2i)
    {
        i2n[pair.second] = pair.first;
    }
    auto init_mk = ct.marking();

    auto rg = genReducedReachGraph(srn, init_mk);
    std::cout << "marking count: " << rg.nodeMarkings.size() << std::endl;
    auto w = 1.0;
    auto tol = 1e-6;
    uint max_iter = 1000;

    SrnSteadyStateSol sol =
        srnSteadyStateSor(rg.graph, rg.edgeRates, w, tol, max_iter);

    auto vscc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                   srnPlaceTokenFunc(n2i.at("p_vscc")));
    auto mvcc_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                   srnPlaceTokenFunc(n2i.at("p_mvcc")));
    auto ledger_qlen = srnProbReward(srn, sol, rg.nodeMarkings,
                                     srnPlaceTokenFunc(n2i.at("p_ledger")));

    auto p_vscc = n2i.at("p_vscc");
    auto vscc_util = srnProbReward(
        srn, sol, rg.nodeMarkings, [vscc_th, p_vscc](PetriNetState st) {
            uint ntoken = st.marking->nToken(p_vscc);
            if (ntoken <= vscc_th)
            {
                return (Real)ntoken / (Real)(vscc_th);
            }
            else
            {
                return 1.0;
            }
        });

    auto t_mvcc = tn2i.at("t_mvcc");
    auto mvcc_util =
        srnProbReward(srn, sol, rg.nodeMarkings, [t_mvcc](PetriNetState st) {
            return st.net->isTransitionEnabled(t_mvcc, st.marking) ? 1.0
                                                                   : 0.0;
        });

    auto ledger_util = srnProbReward(
        srn, sol, rg.nodeMarkings, [t_ledger](PetriNetState st) {
            return st.net->isTransitionEnabled(t_ledger, st.marking) ? 1.0
                                                                     : 0.0;
        });

    std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
    std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
    std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
    std::cout << "vscc_util: " << vscc_util << std::endl;
    std::cout << "mvcc_util: " << mvcc_util << std::endl;
    std::cout << "ledger_util: " << ledger_util << std::endl;
    std::cout << std::endl;
}
