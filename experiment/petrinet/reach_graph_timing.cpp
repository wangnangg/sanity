#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"
#include "splinear.hpp"
#include "timer.hpp"

using namespace sanity::petrinet;
TEST(reach_graph_timing, many_parallel)
{
    SrnCreator crt;
    uint site_count = 22;
    for (uint i = 0; i < site_count; i++)
    {
        auto up = crt.place(1);
        auto down = crt.place(0);
        crt.expTrans(1.0).iarc(up).oarc(down);
        crt.expTrans(2.0).iarc(down).oarc(up);
    }
    auto init_marking = crt.marking();
    auto srn = crt.create();

    timer gent("gen_reach");
    gent.start();
    auto rg = genReducedReachGraph(srn, init_marking);
    gent.whatTime();
    std::cout << "# of markings: " << rg.graph.nodeCount() << std::endl;
    ASSERT_EQ(rg.graph.nodeCount(), std::pow(2, site_count));
}

TEST(reach_graph_timing, multi_stage)
{
    SrnCreator crt;
    uint site_count = 5;
    uint max_job = 24;
    crt.expTrans(1.0).oarc(0).harc(0, max_job);
    crt.place(0);
    for (uint i = 1; i < site_count; i++)
    {
        crt.expTrans(1.0).iarc(i - 1).oarc(i).harc(i, max_job);
        crt.place(1);
    }
    crt.expTrans(1.0).iarc(site_count - 1);

    auto init_marking = crt.marking();
    auto srn = crt.create();

    timer gent("gen_reach");
    gent.start();
    auto rg = genReducedReachGraph(srn, init_marking);
    gent.whatTime();
    std::cout << "# of markings: " << rg.graph.nodeCount() << std::endl;
    ASSERT_EQ(rg.graph.nodeCount(), std::pow(max_job + 1, site_count));
}

TEST(reach_graph_timing, many_parallel_bit)
{
    SrnCreator crt;
    uint site_count = 22;
    for (uint i = 0; i < site_count; i++)
    {
        auto up = crt.place(1);
        auto down = crt.place(0);
        crt.expTrans(1.0).iarc(up).oarc(down);
        crt.expTrans(2.0).iarc(down).oarc(up);
    }
    auto init_marking = crt.bitMarking();
    auto srn = crt.create();

    timer gent("gen_reach");
    gent.start();
    auto rg = genReducedReachGraph(srn, init_marking);
    gent.whatTime();
    std::cout << "# of markings: " << rg.graph.nodeCount() << std::endl;
    ASSERT_EQ(rg.graph.nodeCount(), std::pow(2, site_count));
}

TEST(reach_graph_timing, many_parallel_byte)
{
    SrnCreator crt;
    uint site_count = 22;
    for (uint i = 0; i < site_count; i++)
    {
        auto up = crt.place(1);
        auto down = crt.place(0);
        crt.expTrans(1.0).iarc(up).oarc(down);
        crt.expTrans(2.0).iarc(down).oarc(up);
    }
    auto init_marking = crt.byteMarking();
    auto srn = crt.create();

    timer gent("gen_reach");
    gent.start();
    auto rg = genReducedReachGraph(srn, init_marking);
    gent.whatTime();
    std::cout << "# of markings: " << rg.graph.nodeCount() << std::endl;
    ASSERT_EQ(rg.graph.nodeCount(), std::pow(2, site_count));
}
