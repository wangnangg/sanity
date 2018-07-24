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

std::tuple<GpnCreator, std::unordered_map<std::string, uint>,
           std::unordered_map<std::string, uint>>
fabricModel(Real client_arrival_rate, uint client_queue_len,
            Real client_sdk_process_rate, Real endorse_rate, uint endorse_max,
            Real cons_rate, uint block_size, Real validate_rate, uint vscc_th,
            Real mvcc_rate, Real ledger_rate)
{
    GpnCreator ct;
    std::unordered_map<std::string, uint> n2i;
    std::unordered_map<std::string, uint> tn2i;
    auto c0 = ct.place();
    n2i["c0"] = c0;

    auto p0 = ct.place();
    n2i["p0"] = p0;

    auto p0_end = ct.place();
    n2i["p0_end"] = p0_end;

    auto p1 = ct.place();
    n2i["p1"] = p1;

    auto p1_end = ct.place();
    n2i["p1_end"] = p1_end;

    auto tx_all = ct.place();
    n2i["tx_all"] = tx_all;

    auto p_vscc = ct.place();
    n2i["p_vscc"] = p_vscc;

    auto p_vscc_end = ct.place();
    n2i["p_vscc_end"] = p_vscc_end;

    auto p_mvcc = ct.place();
    n2i["p_mvcc"] = p_mvcc;

    auto p_ledger = ct.place();
    n2i["p_ledger"] = p_ledger;

    uint c0_tx = ct.expTrans(client_arrival_rate)
                     .oarc(c0)
                     .harc(c0, client_queue_len)
                     .idx();
    tn2i["c0_tx"] = c0_tx;

    uint c0_pr = ct.expTrans([c0, client_sdk_process_rate](PetriNetState st) {
                       uint njob = st.marking->nToken(c0);
                       return njob * client_sdk_process_rate;
                   })
                     .iarc(c0)
                     .oarc(p0)
                     .oarc(p1)
                     .idx();
    tn2i["c0_pr"] = c0_pr;

    uint t0_en = ct.expTrans([=](PetriNetState st) {
                       uint ncpu =
                           std::min(st.marking->nToken(p0), endorse_max);
                       return (Real)ncpu * endorse_rate;
                   })
                     .iarc(p0)
                     .oarc(p0_end)
                     .idx();
    tn2i["t0_en"] = t0_en;

    uint t1_end = ct.expTrans([=](PetriNetState st) {
                        uint ncpu =
                            std::min(st.marking->nToken(p1), endorse_max);
                        return (Real)ncpu * endorse_rate;
                    })
                      .iarc(p1)
                      .oarc(p1_end)
                      .idx();
    uint c0_i = ct.immTrans().iarc(p0_end).iarc(p1_end).oarc(tx_all).idx();
    tn2i["c0_i"] = c0_i;

    uint t_cons = ct.expTrans(cons_rate)
                      .iarc(tx_all, block_size)
                      .oarc(p_vscc, block_size)
                      .idx();
    tn2i["t_cons"] = t_cons;

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

TEST(fabric, max_thruput)
{
    uint block_size = (uint)std::stoi(global_argv[1]);
    uint endorse_max = (uint)std::stoi(global_argv[2]);
    Real lambdaC = std::stod(global_argv[3]);

    std::cout << "block size: " << block_size
              << ", endorse_max: " << endorse_max << ", lambda_c: " << lambdaC
              << std::endl;

    std::map<uint, Real> cons_rate = {{40, 75.74}, {80, 81.6}, {120, 93.56}};
    std::map<uint, Real> mvcc_rate = {{40, 2.559}, {80, 5.1}, {120, 7.202}};
    std::map<uint, Real> ledger_rate = {
        {40, 207.8}, {80, 208.3}, {120, 188.4}};
    uint vscc_th = 4;
    auto [ct, n2i, tn2i] =
        fabricModel(lambdaC,                       // client_arrival_rate
                    1000000,                       // client queue length
                    1e3 / 6.469,                   // client_sdk_process_rate
                    1e3 / 3.249,                   // endorse_rate
                    endorse_max,                   // endorse_max
                    1e3 / cons_rate[block_size],   // cons_rate
                    block_size,                    // block_size
                    1e3 / 2.524,                   // validate_rate
                    vscc_th,                       // vscc_th
                    1e3 / mvcc_rate[block_size],   // mvcc_rate
                    1e3 / ledger_rate[block_size]  // ledger_rate
        );
    uint t_ledger = tn2i["t_ledger"];
    auto gpn = ct.create();
    std::vector<std::string> i2n(gpn.placeCount());
    for (const auto& pair : n2i)
    {
        i2n[pair.second] = pair.first;
    }
    auto init_mk = ct.marking();

    auto sim = gpnSimulator(gpn, init_mk, UniformSampler());

    Real start = 0;
    Real end = 1000;

    Real lrate = 1e3 / ledger_rate[block_size] * block_size;
    auto thruput_ob =
        GpnObProbReward(start, end, [t_ledger, lrate](PetriNetState st) {
            if (st.net->isTransitionEnabled(t_ledger, st.marking))
            {
                return lrate;
            }
            else
            {
                return 0.0;
            }
        });
    sim.addObserver(thruput_ob);

    uint p0 = n2i["p0"];
    auto peer0_en_util =
        GpnObProbReward(start, end, [endorse_max, p0](PetriNetState st) {
            uint ntoken = st.marking->nToken(p0);
            if (ntoken <= endorse_max)
            {
                return (Real)ntoken / (Real)(endorse_max);
            }
            else
            {
                return 1.0;
            }
        });
    sim.addObserver(peer0_en_util);

    uint p1 = n2i["p1"];
    auto peer1_en_util =
        GpnObProbReward(start, end, [endorse_max, p1](PetriNetState st) {
            uint ntoken = st.marking->nToken(p1);
            if (ntoken <= endorse_max)
            {
                return (Real)ntoken / (Real)(endorse_max);
            }
            else
            {
                return 1.0;
            }
        });
    sim.addObserver(peer1_en_util);

    auto transmit_util =
        GpnObProbReward(start, end, gpnTransEnabledFunc(tn2i["c0_i"]));
    sim.addObserver(transmit_util);

    auto osn_util =
        GpnObProbReward(start, end, gpnTransEnabledFunc(tn2i["t_cons"]));
    sim.addObserver(osn_util);

    uint p_vscc = n2i["p_vscc"];
    auto vscc_util =
        GpnObProbReward(start, end, [vscc_th, p_vscc](PetriNetState st) {
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
    sim.addObserver(vscc_util);

    auto mvcc_util =
        GpnObProbReward(start, end, gpnTransEnabledFunc(tn2i["t_mvcc"]));
    sim.addObserver(mvcc_util);

    auto ledger_util =
        GpnObProbReward(start, end, gpnTransEnabledFunc(tn2i["t_ledger"]));
    sim.addObserver(ledger_util);

    uint nSample = 100;
    for (uint i = 0; i < nSample; i++)
    {
        sim.begin();
        sim.runFor(end);
        sim.end();
    }

    auto thruput_itv = confidenceInterval(thruput_ob.samples(), 0.95);
    std::cout << "Thruput: " << thruput_itv << std::endl;

    auto peer0_util_itv = confidenceInterval(peer0_en_util.samples(), 0.95);
    std::cout << "Peer0 En Util: " << peer0_util_itv << std::endl;

    auto peer1_util_itv = confidenceInterval(peer1_en_util.samples(), 0.95);
    std::cout << "Peer1 En Util: " << peer1_util_itv << std::endl;

    auto trans_util_itv = confidenceInterval(transmit_util.samples(), 0.95);
    std::cout << "Transmit Util: " << trans_util_itv << std::endl;

    auto osn_util_itv = confidenceInterval(osn_util.samples(), 0.95);
    std::cout << "OSN Util: " << osn_util_itv << std::endl;

    auto vscc_util_itv = confidenceInterval(vscc_util.samples(), 0.95);
    std::cout << "VSCC Util: " << vscc_util_itv << std::endl;

    auto mvcc_util_itv = confidenceInterval(mvcc_util.samples(), 0.95);
    std::cout << "MVCC Util: " << mvcc_util_itv << std::endl;

    auto ledger_util_itv = confidenceInterval(ledger_util.samples(), 0.95);
    std::cout << "LedgerWrite Util: " << ledger_util_itv << std::endl;

    auto utils =
        std::vector<Real>({peer0_util_itv.center(), peer1_util_itv.center(),
                           trans_util_itv.center(), osn_util_itv.center(),
                           vscc_util_itv.center(), mvcc_util_itv.center(),
                           ledger_util_itv.center()});
    auto max_util = std::max_element(utils.begin(), utils.end());
    std::cout << "max util: " << *max_util << std::endl;
}
