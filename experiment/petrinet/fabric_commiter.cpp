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
using namespace sanity::simulate;

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
                      .oarc(p_vscc_end)
                      .idx();
    tn2i["t_vscc"] = t_vscc;

    uint t_block_pack =
        ct.immTrans().iarc(p_vscc_end, block_size).oarc(p_mvcc).idx();

    uint t_mvcc = ct.expTrans(mvcc_rate).iarc(p_mvcc).oarc(p_ledger).idx();
    tn2i["t_mvcc"] = t_mvcc;

    uint t_ledger = ct.expTrans(ledger_rate).iarc(p_ledger).idx();
    tn2i["t_ledger"] = t_ledger;

    return std::make_tuple(std::move(ct), n2i, tn2i);
}

extern char** global_argv;

TEST(fabric, commiter)
{
    Real block_arr_rate = std::stod(global_argv[1]);

    std::cout << "block arrival rate: " << block_arr_rate << std::endl;

    std::map<uint, Real> cons_rate = {{40, 75.74}, {80, 81.6}, {120, 93.56}};
    std::map<uint, Real> mvcc_rate = {{40, 2.559}, {80, 5.1}, {120, 7.202}};
    std::map<uint, Real> ledger_rate = {
        {40, 207.8}, {80, 208.3}, {120, 188.4}};
    uint vscc_th = 4;
    uint block_size = 1;
    auto [ct, n2i, tn2i] =
        commiterModel(block_arr_rate,                // block_arr_rate
                      block_size,                    // block_size
                      1e3 / 2.524,                   // validate_rate
                      vscc_th,                       // vscc_th
                      1e3 / mvcc_rate[block_size],   // mvcc_rate
                      1e3 / ledger_rate[block_size]  // ledger_rate
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
    std::cout << "vscc_qlen: " << vscc_qlen << std::endl;
    std::cout << "mvcc_qlen: " << mvcc_qlen << std::endl;
    std::cout << "ledger_qlen: " << ledger_qlen << std::endl;
}
