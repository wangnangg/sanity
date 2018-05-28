#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"

using namespace sanity::petrinet;
using namespace sanity::simulate;
using namespace sanity::linear;
TEST(petrinet, gspn_create)
{
    GpnCreator creator;
    creator.place(1);
    creator.place(0);
    creator.expTrans(1.0).iarc(0).oarc(1);
    creator.expTrans(2.0).iarc(1).oarc(0);
    auto gpn = creator.create();
    ASSERT_EQ(gpn.pnet.transCount(), 2);
    ASSERT_EQ(gpn.pnet.placeCount(), 2);
    ASSERT_EQ(gpn.transProps.size(), 2);
}

TEST(petrinet, gspn_simulate_2place)
{
    GpnCreator creator;
    creator.place(1);
    creator.place();
    creator.expTrans(1.0).iarc(0).oarc(1);
    creator.expTrans(2.0).iarc(1).oarc(0);
    auto gpn = creator.create();
    Marking init = creator.marking();
    auto sim = gpnSimulator(gpn, init, UniformSampler());
    auto p0 = GpnObPlaceToken(0, 0, 1000.0);
    auto p1 = GpnObPlaceToken(1, 0, 1000.0);
    sim.addObserver(p0);
    sim.addObserver(p1);
    sim.begin();
    sim.runFor(1000.0);
    sim.end();
    ASSERT_GT(p0.samples()[0], p1.samples()[0]);
    std::cout << "place 0 avg token: " << p0.samples()[0] << std::endl;
    std::cout << "place 1 avg token: " << p1.samples()[0] << std::endl;
}

TEST(petrinet, gspn_simulate_two_server_queue)
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
        auto ob_queue = GpnObPlaceToken(queue, start, end);
        auto ob_done = GpnObPlaceToken(done, start, end);
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

        auto rg = genReducedReachGraph(srn, init, 1e-6, 100);
        auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
        auto prob = Vector(rg.nodeMarkings.size(), 1.0);
        auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
        ASSERT_LT(iter.error, 1e-6);

        std::cout << "prob: " << prob << std::endl;
        Real avg_queue = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_queue += mk.nToken(queue) * prob(i);
        }
        std::cout << "queue avg token in srn: " << avg_queue << std::endl;

        Real avg_done = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_done += mk.nToken(done) * prob(i);
        }
        std::cout << "done avg token in srn: " << avg_done << std::endl;
    }
}

TEST(petrinet, gspn_simulate_two_server_queue_confidence)
{
    Interval queue_itv, done_itv;
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
        Real end = 1000;
        uint nSample = 100;
        auto ob_queue = GpnObPlaceToken(queue, start, end);
        auto ob_done = GpnObPlaceToken(done, start, end);
        auto log = GpnObLog(false);
        sim.addObserver(ob_queue);
        sim.addObserver(ob_done);
        sim.addObserver(log);

        for (uint i = 0; i < nSample; i++)
        {
            sim.begin();
            sim.runFor(end);
            sim.end();
        }

        ASSERT_EQ(ob_queue.samples().size(), nSample);
        ASSERT_EQ(ob_done.samples().size(), nSample);
        queue_itv = confidenceInterval(ob_queue.samples(), 0.95);
        done_itv = confidenceInterval(ob_done.samples(), 0.95);
        std::cout << "place queue avg token: " << queue_itv << std::endl;
        std::cout << "place done avg token: " << done_itv << std::endl;
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

        auto rg = genReducedReachGraph(srn, init, 1e-6, 100);
        auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
        auto prob = Vector(rg.nodeMarkings.size(), 1.0);
        auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
        ASSERT_LT(iter.error, 1e-6);

        std::cout << "prob: " << prob << std::endl;
        Real avg_queue = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_queue += mk.nToken(queue) * prob(i);
        }
        std::cout << "queue avg token in srn: " << avg_queue << std::endl;
        ASSERT_LT(queue_itv.begin, avg_queue);
        ASSERT_GT(queue_itv.end, avg_queue);
        Real avg_done = 0;
        for (uint i = 0; i < rg.nodeMarkings.size(); i++)
        {
            const auto& mk = rg.nodeMarkings[i];
            avg_done += mk.nToken(done) * prob(i);
        }
        std::cout << "done avg token in srn: " << avg_done << std::endl;
        ASSERT_LT(done_itv.begin, avg_done);
        ASSERT_GT(done_itv.end, avg_done);
    }
}
