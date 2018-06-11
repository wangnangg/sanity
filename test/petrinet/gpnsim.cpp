#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"

using namespace sanity::petrinet;
using namespace sanity::simulate;
using namespace sanity::linear;

TEST(petrinet, gpn_simulate_2place)
{
    GpnCreator creator;
    creator.place(1);
    creator.place();
    creator.expTrans(1.0).iarc(0).oarc(1);
    creator.expTrans(2.0).iarc(1).oarc(0);
    auto gpn = creator.create();
    Marking init = creator.marking();
    auto sim = gpnSimulator(gpn, init, UniformSampler());
    auto p0 = GpnObProbReward(0, 1000.0, [](PetriNetState state) {
        return state.marking->nToken(0);
    });
    auto p1 = GpnObProbReward(0, 1000.0, [](PetriNetState state) {
        return state.marking->nToken(1);
    });

    sim.addObserver(p0);
    sim.addObserver(p1);

    for (uint i = 0; i < 100; i++)
    {
        sim.begin();
        sim.runFor(1000.0);
        sim.end();
    }
    auto p0_itv = confidenceInterval(p0.samples(), 0.95);
    auto p1_itv = confidenceInterval(p1.samples(), 0.95);
    ASSERT_GT(p0_itv.center(), p1_itv.center());
    std::cout << "place 0 token: " << p0_itv << std::endl;
    std::cout << "place 1 token: " << p1_itv << std::endl;
}

TEST(petrinet, gpn_simulate_2place_g_enable)
{
    GpnCreator creator;
    creator.place(1);
    creator.place();
    creator.expTrans(1.0).iarc(0).oarc(1);
    creator.expTrans(2.0).iarc(1).oarc(0);
    creator.globalEnable(
        [](auto state) { return gpnPlaceToken(state, 0) == 1; });
    auto gpn = creator.create();
    Marking init = creator.marking();
    auto sim = gpnSimulator(gpn, init, UniformSampler());
    auto p0 = GpnObProbReward(0, 1000.0, [](PetriNetState state) {
        return state.marking->nToken(0);
    });
    auto p1 = GpnObProbReward(0, 1000.0, [](PetriNetState state) {
        return state.marking->nToken(1);
    });

    auto evt_log = GpnObLog(true);
    auto evt_counter = GpnObEventCounter();

    sim.addObserver(p0);
    sim.addObserver(p1);
    sim.addObserver(evt_log);
    sim.addObserver(evt_counter);
    for (uint i = 0; i < 100; i++)
    {
        sim.begin();
        sim.runFor(1000.0);
        sim.end();
        evt_log.off();
    }
    ASSERT_EQ(evt_counter.samples()[0], 3);
    auto p0_itv = confidenceInterval(p0.samples(), 0.95);
    auto p1_itv = confidenceInterval(p1.samples(), 0.95);
    ASSERT_LT(p0_itv.center(), p1_itv.center());
    std::cout << "place 0 token: " << p0_itv << std::endl;
    std::cout << "place 1 token: " << p1_itv << std::endl;
}

TEST(petrinet, gpn_simulate_2place_bitmarking)
{
    GpnCreator creator;
    creator.place(1);
    creator.place();
    creator.expTrans(1.0).iarc(0).oarc(1);
    creator.expTrans(2.0).iarc(1).oarc(0);
    auto gpn = creator.create();
    auto init = creator.bitMarking();
    auto sim = gpnSimulator(gpn, init, UniformSampler());
    auto p0 = GpnObProbReward(0, 1000.0, [](PetriNetState state) {
        return state.marking->nToken(0);
    });
    auto p1 = GpnObProbReward(0, 1000.0, [](PetriNetState state) {
        return state.marking->nToken(1);
    });

    sim.addObserver(p0);
    sim.addObserver(p1);

    for (uint i = 0; i < 100; i++)
    {
        sim.begin();
        sim.runFor(1000.0);
        sim.end();
    }
    auto p0_itv = confidenceInterval(p0.samples(), 0.95);
    auto p1_itv = confidenceInterval(p1.samples(), 0.95);
    ASSERT_GT(p0_itv.center(), p1_itv.center());
    std::cout << "place 0 token: " << p0_itv << std::endl;
    std::cout << "place 1 token: " << p1_itv << std::endl;
}

TEST(petrinet, gpn_simulate_two_server_queue)
{
    {
        GpnCreator creator;
        auto queue = creator.place(0);
        auto done = creator.place(4);
        creator.expTrans(1.0).iarc(queue).oarc(done);
        creator.expTrans(2.0).iarc(queue).oarc(done);
        creator.expTrans(1.0).iarc(done).oarc(queue);

        auto gpn = creator.create();
        Marking init = creator.marking();

        auto sim = gpnSimulator(gpn, init, UniformSampler());

        Real start = 0;
        Real end = 10000;
        auto ob_queue = GpnObProbReward(start, end, [queue](auto state) {
            return state.marking->nToken(queue);
        });
        auto ob_done = GpnObProbReward(start, end, [done](auto state) {
            return state.marking->nToken(done);
        });
        auto log = GpnObLog(false);
        sim.addObserver(ob_queue);
        sim.addObserver(ob_done);
        sim.addObserver(log);

        sim.begin();
        sim.runFor(end);
        sim.end();

        std::cout << "place queue avg token: " << ob_queue.samples()[0]
                  << std::endl;
        std::cout << "place done avg token: " << ob_done.samples()[0]
                  << std::endl;
    }
    {
        SrnCreator creator;
        auto queue = creator.place(4);
        auto done = creator.place();
        creator.expTrans(1.0).iarc(queue).oarc(done);
        creator.expTrans(2.0).iarc(queue).oarc(done);
        creator.expTrans(1.0).iarc(done).oarc(queue);

        auto srn = creator.create();
        Marking init = creator.marking();

        auto rg = genReducedReachGraph(srn, init);
        auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
        auto prob = Vector(rg.nodeMarkings.size(), 1.0);
        auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
        ASSERT_LT(iter.error, 1e-6);

        std::cout << "prob: " << prob << std::endl;
        Real avg_queue = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_queue += mk->nToken(queue) * prob(i);
        }
        std::cout << "queue avg token in srn: " << avg_queue << std::endl;

        Real avg_done = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_done += mk->nToken(done) * prob(i);
        }
        std::cout << "done avg token in srn: " << avg_done << std::endl;
    }
}

TEST(petrinet, gpn_simulate_two_server_queue_confidence)
{
    Interval queue_itv;
    {
        GpnCreator creator;
        auto queue = creator.place(1);
        auto done = creator.place(3);
        creator.expTrans(1.0).iarc(queue).oarc(done);
        creator.expTrans(2.0).iarc(queue).oarc(done);
        creator.expTrans(1.0).iarc(done).oarc(queue);

        auto gpn = creator.create();
        Marking init = creator.marking();

        auto sim = gpnSimulator(gpn, init, UniformSampler());

        Real start = 100;
        Real end = 200;
        uint nSample = 100;
        auto ob_queue = GpnObProbReward(start, end, [queue](auto state) {
            return state.marking->nToken(queue);
        });

        auto log = GpnObLog(false);
        sim.addObserver(ob_queue);
        sim.addObserver(log);

        for (uint i = 0; i < nSample; i++)
        {
            sim.begin();
            sim.runFor(end);
            sim.end();
            if ((i + 1) % 30 == 0)
            {
                std::cout << "place queue avg token: "
                          << confidenceInterval(ob_queue.samples(), 0.99)
                          << std::endl;
            }
        }

        ASSERT_EQ(ob_queue.samples().size(), nSample);
        queue_itv = confidenceInterval(ob_queue.samples(), 0.99);
        std::cout << "place queue avg token: " << queue_itv << std::endl;
    }
    {
        SrnCreator creator;
        auto queue = creator.place(4);
        auto done = creator.place();
        creator.expTrans(1.0).iarc(queue).oarc(done);
        creator.expTrans(2.0).iarc(queue).oarc(done);
        creator.expTrans(1.0).iarc(done).oarc(queue);

        auto srn = creator.create();
        Marking init = creator.marking();

        auto rg = genReducedReachGraph(srn, init);
        auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
        auto prob = Vector(rg.nodeMarkings.size(), 1.0);
        auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
        ASSERT_LT(iter.error, 1e-6);

        std::cout << "prob: " << prob << std::endl;
        Real avg_queue = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_queue += mk->nToken(queue) * prob(i);
        }
        std::cout << "queue avg token in srn: " << avg_queue << std::endl;
        ASSERT_LT(queue_itv.begin, avg_queue);
        ASSERT_GT(queue_itv.end, avg_queue);
    }
}

TEST(petrinet, gpn_simulate_two_server_queue_conversion)
{
    SrnCreator creator;
    auto queue = creator.place(4);
    auto done = creator.place();
    creator.expTrans(1.0).iarc(queue).oarc(done);
    creator.expTrans(2.0).iarc(queue).oarc(done);
    creator.expTrans(1.0).iarc(done).oarc(queue);

    auto srn = creator.create();
    Marking init = creator.marking();

    Interval queue_itv;
    {
        auto gpn = srn2gpn(srn);

        auto sim = gpnSimulator(gpn, init, UniformSampler());

        Real start = 100;
        Real end = 200;
        uint nSample = 100;
        auto ob_queue = GpnObProbReward(start, end, [queue](auto state) {
            return state.marking->nToken(queue);
        });

        auto log = GpnObLog(false);
        sim.addObserver(ob_queue);
        sim.addObserver(log);

        for (uint i = 0; i < nSample; i++)
        {
            sim.begin();
            sim.runFor(end);
            sim.end();
            if ((i + 1) % 30 == 0)
            {
                std::cout << "place queue avg token: "
                          << confidenceInterval(ob_queue.samples(), 0.99)
                          << std::endl;
            }
        }

        ASSERT_EQ(ob_queue.samples().size(), nSample);
        queue_itv = confidenceInterval(ob_queue.samples(), 0.99);
        std::cout << "place queue avg token: " << queue_itv << std::endl;
    }
    {
        auto rg = genReducedReachGraph(srn, init);
        auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
        auto prob = Vector(rg.nodeMarkings.size(), 1.0);
        auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
        ASSERT_LT(iter.error, 1e-6);

        std::cout << "prob: " << prob << std::endl;
        Real avg_queue = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_queue += mk->nToken(queue) * prob(i);
        }
        std::cout << "queue avg token in srn: " << avg_queue << std::endl;
        ASSERT_LT(queue_itv.begin, avg_queue);
        ASSERT_GT(queue_itv.end, avg_queue);
    }
}

TEST(petrinet, gpn_molloy_thesis)
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
    Real end = 100;
    auto mk_p0 = GpnObProbReward(start, end, gpnPlaceTokenFunc(p0));
    auto mk_p3 = GpnObProbReward(start, end, gpnPlaceTokenFunc(p3));
    auto evt_counter = GpnObEventCounter();
    sim.addObserver(mk_p0);
    sim.addObserver(mk_p3);
    sim.addObserver(evt_counter);

    uint nSample = 100;
    for (uint i = 0; i < nSample; i++)
    {
        sim.begin();
        sim.runFor(end);
        sim.end();
    }

    std::cout << "avg # events: "
              << confidenceInterval(evt_counter.samples(), 0.99) << std::endl;
    auto mk_p0_itv = confidenceInterval(mk_p0.samples(), 0.99);
    std::cout << "token in p0: " << mk_p0_itv << std::endl;
    ASSERT_LT(mk_p0_itv.begin, 0.4497);
    ASSERT_GT(mk_p0_itv.end, 0.4497);

    auto mk_p3_itv = confidenceInterval(mk_p3.samples(), 0.99);
    std::cout << "token in p3: " << mk_p3_itv << std::endl;
    ASSERT_LT(mk_p3_itv.begin, 0.1);
    ASSERT_GT(mk_p3_itv.end, 0.1);
}

/*
  This example corresponds to the following piece of software:

  A:  statements;
  PARBEGIN
  B1:  statements;    B2:  IF cond THEN
  C:  statements;
  ELSE
  DO
  D:  statements
  WHILE cond;
  IFEND
  PAREND
*/
TEST(petrinet, gpn_software_mtta)
{
    GpnCreator ct;
    uint p0 = ct.place(4);
    uint p1 = ct.place();
    uint p2 = ct.place();
    uint p3 = ct.place();
    uint p4 = ct.place();
    uint p5 = ct.place();
    uint p6 = ct.place();
    uint p7 = ct.place();
    uint p8 = ct.place();

    uint t2 = ct.immTrans(0.4).iarc(p3).oarc(p4).idx();
    uint t3 = ct.immTrans(0.6).iarc(p3).oarc(p5).idx();
    uint t6 = ct.immTrans(0.05).iarc(p7).oarc(p6).idx();
    uint t7 = ct.immTrans(0.95).iarc(p7).oarc(p5).idx();
    uint t8 = ct.immTrans(1.0).iarc(p2).iarc(p6).oarc(p8).idx();
    uint A = ct.expTrans(1.0).iarc(p0).oarc(p1).oarc(p3).idx();
    uint B1 = ct.expTrans(0.3).iarc(p1).oarc(p2).idx();
    uint C = ct.expTrans(0.2).iarc(p4).oarc(p6).idx();
    uint D = ct.expTrans(7.0).iarc(p5).oarc(p7).idx();

    auto gpn = ct.create();
    auto mk = ct.byteMarking();

    auto sim = gpnSimulator(gpn, mk, UniformSampler());

    auto mtta_ob = GpnObMtta();
    sim.addObserver(mtta_ob);

    uint nSample = 1000;
    for (uint i = 0; i < nSample; i++)
    {
        sim.begin();
        sim.runTillEnd();
        sim.end();
    }

    auto itv = confidenceInterval(mtta_ob.samples(), 0.99);
    std::cout << "mtta: " << itv << std::endl;
    ASSERT_LT(itv.begin, 17.6701);
    ASSERT_GT(itv.end, 17.6701);
}

TEST(petrinet, converted_gpn_software_mtta)
{
    SrnCreator ct;
    uint p0 = ct.place(4);
    uint p1 = ct.place();
    uint p2 = ct.place();
    uint p3 = ct.place();
    uint p4 = ct.place();
    uint p5 = ct.place();
    uint p6 = ct.place();
    uint p7 = ct.place();
    uint p8 = ct.place();

    uint t2 = ct.immTrans(0.4).iarc(p3).oarc(p4).idx();
    uint t3 = ct.immTrans(0.6).iarc(p3).oarc(p5).idx();
    uint t6 = ct.immTrans(0.05).iarc(p7).oarc(p6).idx();
    uint t7 = ct.immTrans(0.95).iarc(p7).oarc(p5).idx();
    uint t8 = ct.immTrans(1.0).iarc(p2).iarc(p6).oarc(p8).idx();
    uint A = ct.expTrans(1.0).iarc(p0).oarc(p1).oarc(p3).idx();
    uint B1 = ct.expTrans(0.3).iarc(p1).oarc(p2).idx();
    uint C = ct.expTrans(0.2).iarc(p4).oarc(p6).idx();
    uint D = ct.expTrans(7.0).iarc(p5).oarc(p7).idx();

    auto gpn = srn2gpn(ct.create());
    auto mk = ct.byteMarking();

    auto sim = gpnSimulator(gpn, mk, UniformSampler());

    auto mtta_ob = GpnObMtta();
    sim.addObserver(mtta_ob);

    uint nSample = 1000;
    for (uint i = 0; i < nSample; i++)
    {
        sim.begin();
        sim.runTillEnd();
        sim.end();
    }

    auto itv = confidenceInterval(mtta_ob.samples(), 0.99);
    std::cout << "mtta: " << itv << std::endl;
    ASSERT_LT(itv.begin, 17.6701);
    ASSERT_GT(itv.end, 17.6701);
}
