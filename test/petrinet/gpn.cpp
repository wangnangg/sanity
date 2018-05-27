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
    auto p0 = GpnObPlaceNonEmpty(0);
    auto p1 = GpnObPlaceNonEmpty(1);
    sim.addObserver(p0);
    sim.addObserver(p1);
    sim.begin();
    sim.runFor(1000.0);
    sim.end();
    std::cout << "place 0 non empty: " << p0.prob() << ", " << p0.accSojTime()
              << std::endl;
    std::cout << "place 1 non empty: " << p1.prob() << ", " << p1.accSojTime()
              << std::endl;
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

        auto ob_queue = GpnObPlaceToken(queue);
        auto ob_done = GpnObPlaceToken(done);
        auto ob_busy = GpnObPlaceNonEmpty(queue);
        auto log = GpnObLog(false);
        sim.addObserver(ob_queue);
        sim.addObserver(ob_done);
        sim.addObserver(ob_busy);
        sim.addObserver(log);

        sim.begin();
        sim.runFor(1000.0);
        sim.end();

        std::cout << "place queue avg token: " << ob_queue.avgToken()
                  << std::endl;
        std::cout << "place done avg token: " << ob_done.avgToken()
                  << std::endl;
        std::cout << "busy: " << ob_busy.prob() << std::endl;
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
        std::cout << "avg token in srn: " << avg_queue << std::endl;
    }
}
