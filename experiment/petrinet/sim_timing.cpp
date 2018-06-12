#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"
#include "simulate.hpp"
#include "splinear.hpp"
#include "timer.hpp"

using namespace sanity::petrinet;
using namespace sanity::simulate;
TEST(sim_timing, molloy_thesis)
{
    SrnCreator ct;
    auto p0 = ct.place(1);
    auto p1 = ct.place();
    auto p2 = ct.place();
    auto p3 = ct.place();
    auto p4 = ct.place();

    auto t0 = ct.expTrans(1.0).iarc(p0).oarc(p1).oarc(p2).idx();
    auto t1 = ct.expTrans(3.0).iarc(p1).oarc(p3).idx();
    auto t2 = ct.expTrans(7.0).iarc(p2).oarc(p4).idx();
    auto t3 = ct.expTrans(9.0).iarc(p3).oarc(p1).idx();
    auto t4 = ct.expTrans(5.0).iarc(p3).iarc(p4).oarc(p0).idx();

    auto srn = ct.create();
    auto mk = ct.marking();

    auto gpn = srn2gpn(srn);

    auto sim = gpnSimulator(gpn, mk, UniformSampler());

    Real start = 0;
    Real end = 10000;
    auto mk_p0 = GpnObProbReward(start, end, gpnPlaceTokenFunc(p0));
    auto mk_p3 = GpnObProbReward(start, end, gpnPlaceTokenFunc(p3));
    auto evt_counter = GpnObEventCounter();
    sim.addObserver(mk_p0);
    sim.addObserver(mk_p3);
    sim.addObserver(evt_counter);

    uint nSample = 1000;
    for (uint i = 0; i < nSample; i++)
    {
        sim.begin();
        sim.runFor(end);
        sim.end();
    }
}
