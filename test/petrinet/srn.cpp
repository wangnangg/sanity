#include <gtest/gtest.h>
#include "petrinet.hpp"
using namespace sanity::petrinet;
using namespace sanity::linear;

TEST(petrinet, srn_create)
{
    SRNCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();
    ASSERT_EQ(srn.pnet.transCount(), 2);
    ASSERT_EQ(srn.pnet.placeCount(), 2);
    ASSERT_EQ(srn.transProps.size(), 2);
}

TEST(petrinet, srn_reach_gen)
{
    SRNCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 2);
    auto rg = genReachGraph(srn.pnet, initmk);
    ASSERT_EQ(rg.digraph.nodeCount(), 3);
    ASSERT_EQ(rg.markings.size(), 3);
    ASSERT_EQ(rg.edge2trans.size(), 4);
}

TEST(petrinet, srn_ratemat)
{
    SRNCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 2);
    auto rg = genReachGraph(srn.pnet, initmk);

    auto trimat = srnRateMatrix(srn, rg, Permutation(rg.digraph.nodeCount()));
    auto spmat = triple2compressed(trimat, Spmatrix::RowCompressed);
    std::cout << spmat << std::endl;
}

TEST(petrinet, srn_sor)
{
    SRNCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 2);
    auto rg = genReachGraph(srn.pnet, initmk);

    auto trimat = srnRateMatrix(srn, rg, Permutation(rg.digraph.nodeCount()));
    auto Q = triple2compressed(trimat, Spmatrix::RowCompressed);
    auto prob = Vector(rg.markings.size(), 1.0);
    auto iter = steadyStateSOR(Q, mutableView(prob), 1.0, 1e-6, 1000);
    std::cout << "sor nIter: " << iter.nIter << ", error: " << iter.error
              << std::endl;
    ASSERT_LT(iter.error, 1e-6);
    std::cout << Q << std::endl;
    std::cout << prob << std::endl;
}
