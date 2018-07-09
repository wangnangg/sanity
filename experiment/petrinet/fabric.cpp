#include <gtest/gtest.h>
#include <algorithm>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"
#include "simulate.hpp"
#include "splinear.hpp"
#include "timer.hpp"

using namespace sanity::petrinet;
using namespace sanity::simulate;

GpnCreator fabricModel(Real client_arrival_rate, uint client_queue_size,
                       Real client_sdk_process_rate, Real endorse_rate,
                       uint endorse_max, Real cons_rate, uint block_size,
                       Real validate_rate, uint vscc_th, Real mvcc_rate,
                       Real ledger_rate)
{
    GpnCreator ct;
    auto c0 = ct.place();
    auto p0 = ct.place();
    auto p0_end = ct.place();
    auto p1 = ct.place();
    auto p1_end = ct.place();
    auto tx_all = ct.place();
    auto p_vscc = ct.place();
    auto p_vscc_end = ct.place();
    auto p_mvcc = ct.place();
    auto p_mvcc_end = ct.place();

    uint c0_tx = ct.expTrans(client_arrival_rate)
                     .oarc(c0)
                     .harc(c0, client_queue_size)
                     .idx();
    uint c0_pr =
        ct.expTrans(client_sdk_process_rate).iarc(c0).oarc(p0).oarc(p1).idx();
    uint t0_en = ct.expTrans([=](PetriNetState st) {
                       uint ncpu =
                           std::min(st.marking->nToken(p0), endorse_max);
                       return (Real)ncpu * endorse_rate;
                   })
                     .iarc(p0)
                     .oarc(p0_end)
                     .idx();
    uint t1_end = ct.expTrans([=](PetriNetState st) {
                        uint ncpu =
                            std::min(st.marking->nToken(p1), endorse_max);
                        return (Real)ncpu * endorse_rate;
                    })
                      .iarc(p1)
                      .oarc(p1_end)
                      .idx();
    uint c0_i = ct.immTrans().iarc(p0_end).iarc(p1_end).oarc(tx_all).idx();
    uint t_cons = ct.expTrans(cons_rate)
                      .iarc(tx_all, block_size)
                      .oarc(p_vscc, block_size)
                      .harc(p_vscc, block_size * 2)
                      .idx();
    uint t_vscc = ct.expTrans([=](PetriNetState st) {
                        uint npar =
                            std::min(st.marking->nToken(p_vscc), vscc_th);
                        return (Real)npar * validate_rate;
                    })
                      .iarc(p_vscc)
                      .oarc(p_vscc_end)
                      .idx();
    uint v_i = ct.immTrans()
                   .iarc(p_vscc_end, block_size)
                   .oarc(p_mvcc, block_size)
                   .idx();
    uint t_mvcc = ct.expTrans(mvcc_rate).iarc(p_mvcc).oarc(p_mvcc_end).idx();
    uint t_ledger =
        ct.expTrans(ledger_rate).iarc(p_mvcc_end, block_size).idx();

    return ct;
}

TEST(fabric, test_model)
{
    auto ct = fabricModel(1.2,    // client_arrival_rate
                          10,     // client_queue_size
                          1,      // client_sdk_process_rate
                          0.179,  // endorse_rate
                          16,     // endorse_max
                          0.02,   // cons_rate
                          40,     // block_size
                          0.5,    // validate_rate
                          4,      // vscc_th
                          13.59,  // mvcc_rate
                          0.225   // ledger_rate
    );
    auto gpn = ct.create();
    auto init_mk = ct.marking();

    auto sim = gpnSimulator(gpn, init_mk, UniformSampler());

    Real start = 0;
    Real end = 10000;

    uint nSample = 1000;
    for (uint i = 0; i < nSample; i++)
    {
        sim.begin();
        sim.runFor(end);
        sim.end();
    }
}
