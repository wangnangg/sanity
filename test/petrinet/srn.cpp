#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"
#include "splinear.hpp"
using namespace sanity::petrinet;
using namespace sanity::linear;
using namespace sanity::splinear;

TEST(petrinet, srn_create)
{
    SrnCreator creator;
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
    SrnCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 2);
    auto rg = genReachGraph(srn.pnet, initmk);
    ASSERT_EQ(rg.graph.nodeCount(), 3);
    ASSERT_EQ(rg.nodeMarkings.size(), 3);
    ASSERT_EQ(rg.edgeTrans.size(), 4);
}

TEST(petrinet, srn_ratemat)
{
    SrnCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 2);
    auto rg = genReducedReachGraph(srn, initmk, 1e-6, 1000);

    auto spmat = srnRateMatrix(rg.graph, rg.edgeRates,
                               Permutation(rg.graph.nodeCount()));
    std::cout << spmat << std::endl;
}

TEST(petrinet, srn_sor)
{
    SrnCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.expTrans(1.0).iarc(p1).oarc(p2);
    creator.expTrans(2.0).iarc(p2).oarc(p1);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 2);
    auto rg = genReducedReachGraph(srn, initmk, 1e-6, 1000);

    auto Q = srnRateMatrix(rg.graph, rg.edgeRates,
                           Permutation(rg.graph.nodeCount()));
    auto prob = Vector(rg.nodeMarkings.size(), 1.0);
    auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
    std::cout << "sor nIter: " << iter.nIter << ", error: " << iter.error
              << std::endl;
    ASSERT_LT(iter.error, 1e-6);
    std::cout << Q << std::endl;
    std::cout << prob << std::endl;
    ASSERT_NEAR(prob(0), 1.0 / 7.0 * 4.0, 1e-6);
    ASSERT_NEAR(prob(1), 1.0 / 7.0 * 2.0, 1e-6);
    ASSERT_NEAR(prob(2), 1.0 / 7.0 * 1.0, 1e-6);
}

// initial marking is vanishing
TEST(petrinet, srn_imme_reach_gen1)
{
    SrnCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    creator.immTrans(1.0).iarc(p1).oarc(p2);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 1);
    auto rg = genReducedReachGraph(srn, initmk, 1e-6, 10);
    ASSERT_EQ(rg.graph.nodeCount(), 1);
    ASSERT_EQ(rg.graph.edgeCount(), 0);
    ASSERT_EQ(rg.initProbs[0].prob, 1.0);
}

// initial marking is vanishing, reaching two states
TEST(petrinet, srn_imme_reach_gen2)
{
    SrnCreator creator;
    auto p1 = creator.place();
    auto p2 = creator.place();
    auto p3 = creator.place();
    creator.immTrans(1.0).iarc(p1).oarc(p2);
    creator.immTrans(3.0).iarc(p1).oarc(p3);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p1, 1);
    auto rg = genReducedReachGraph(srn, initmk, 1e-6, 10);
    ASSERT_EQ(rg.graph.nodeCount(), 2);
    ASSERT_EQ(rg.graph.edgeCount(), 0);
    ASSERT_EQ(rg.initProbs[0].prob, 0.25);
    ASSERT_EQ(rg.initProbs[1].prob, 0.75);
}

// initial marking is tangible, a vanishing intermediate state
TEST(petrinet, srn_imme_reach_gen3)
{
    SrnCreator creator;
    auto p0 = creator.place();
    auto p1 = creator.place();
    auto p2 = creator.place();
    auto p3 = creator.place();
    creator.expTrans(2.0).iarc(p0).oarc(p1);
    creator.immTrans(1.0).iarc(p1).oarc(p2);
    creator.immTrans(3.0).iarc(p1).oarc(p3);
    auto srn = creator.create();

    Marking initmk(srn.pnet.placeCount());
    initmk.setToken(p0, 1);
    auto rg = genReducedReachGraph(srn, initmk, 1e-6, 10);
    ASSERT_EQ(rg.graph.nodeCount(), 3);
    ASSERT_EQ(rg.graph.edgeCount(), 2);
    ASSERT_EQ(rg.initProbs[0].prob, 1.0);
    ASSERT_EQ(rg.edgeRates[0], 0.5);
    ASSERT_EQ(rg.edgeRates[1], 1.5);
}

TEST(petrinet, srn_birthdeath_sor)
{
    SrnCreator ct;
    auto p_live = ct.place();
    auto p_dead = ct.place();
    ct.expTrans(1.0).iarc(p_live).oarc(p_dead);
    ct.expTrans(10.0).iarc(p_dead).oarc(p_live);
    auto srn = ct.create();
    Marking mk(2);
    mk.setToken(p_live, 10);
    auto rg = genReducedReachGraph(srn, mk, 1e-6, 100);
    ASSERT_EQ(rg.graph.nodeCount(), 11);
    ASSERT_EQ(rg.graph.edgeCount(), 20);
    auto Q = srnRateMatrix(rg.graph, rg.edgeRates,
                           Permutation(rg.graph.nodeCount()));
    auto prob = Vector(rg.nodeMarkings.size(), 1.0);
    auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
    std::cout << Q << std::endl;
    std::cout << prob << std::endl;
    std::cout << "sor nIter: " << iter.nIter << ", error: " << iter.error
              << std::endl;
    ASSERT_LT(iter.error, 1e-6);
    ASSERT_NEAR(prob(0), 0.9, 1e-6);
}

TEST(petrinet, srn_birthdeath_decomp)
{
    SrnCreator ct;
    auto p_live = ct.place();
    auto p_dead = ct.place();
    ct.expTrans(1.0).iarc(p_live).oarc(p_dead);
    ct.expTrans(10.0).iarc(p_dead).oarc(p_live);
    auto srn = ct.create();
    Marking mk(2);
    mk.setToken(p_live, 10);
    auto rg = genReducedReachGraph(srn, mk, 1e-6, 100);
    ASSERT_EQ(rg.graph.nodeCount(), 11);
    ASSERT_EQ(rg.graph.edgeCount(), 20);

    uint max_iter = 100;
    Real tol = 1e-6;
    Real w = 1;
    auto sol = srnSteadyStateDecomp(
        rg.graph, rg.edgeRates, rg.initProbs,
        [=](const Spmatrix& A, VectorMutableView x, VectorConstView b) {
            auto res = solveSor(A, x, b, w, tol, max_iter);
            if (res.error > tol || std::isnan(res.error))
            {
                std::cout << "Sor. nIter: " << res.nIter;
                std::cout << ", error: " << res.error << std::endl;
                throw std::invalid_argument("Sor failed to converge.");
            }
        });
    std::cout << sol.solution << std::endl;
    std::cout << sol.matrix2node << std::endl;
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(0)), 0.9, 1e-6);
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(1)), 0.09, 1e-6);
}
