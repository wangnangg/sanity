#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"
#include "splinear.hpp"

using namespace sanity::petrinet;
using namespace sanity::linear;
using namespace sanity::splinear;

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
    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 11);
    ASSERT_EQ(rg.graph.edgeCount(), 20);
    auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
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
    auto rg = genReducedReachGraph(srn, mk);
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

TEST(petrinet, srn_trivial_absoring_decomp)
{
    SrnCreator ct;
    auto p_start = ct.place();
    auto p_end = ct.place();
    ct.expTrans(2.0).iarc(p_start).oarc(p_end);
    auto srn = ct.create();

    Marking mk(2);
    mk.setToken(p_start, 1);

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 2);
    ASSERT_EQ(rg.graph.edgeCount(), 1);

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
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(0)), 0.5, 1e-6);
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(1)), 1.0, 1e-6);
}

TEST(petrinet, srn_two_absoring_decomp)
{
    SrnCreator ct;
    auto p_start = ct.place();
    auto p_end1 = ct.place();
    auto p_end2 = ct.place();
    ct.expTrans(2.5).iarc(p_start).oarc(p_end1);
    ct.expTrans(7.5).iarc(p_start).oarc(p_end2);
    auto srn = ct.create();

    Marking mk(srn.placeCount());
    mk.setToken(p_start, 1);

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 3);
    ASSERT_EQ(rg.graph.edgeCount(), 2);

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
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(0)), 0.1, 1e-6);
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(1)), 0.25, 1e-6);
    ASSERT_NEAR(sol.solution((uint)sol.matrix2node.backward(2)), 0.75, 1e-6);
}

static void printSrnSol(
    const std::vector<std::unique_ptr<MarkingIntf>>& markings,
    const Permutation& mat2node, uint ntan, VectorConstView solution)
{
    std::cout << "transients (" << ntan << "):" << std::endl;
    for (uint i = 0; i < ntan; i++)
    {
        const auto& mk = markings[(uint)mat2node.forward(i)];
        std::cout << "( ";
        for (uint j = 0; j < mk->size(); j++)
        {
            std::cout << mk->nToken(j) << ' ';
        }
        std::cout << ")";
        std::cout << " tau:" << solution(i) << std::endl;
    }
    std::cout << "absorbing groups (" << markings.size() - ntan
              << "):" << std::endl;
    for (uint i = ntan; i < markings.size(); i++)
    {
        const auto& mk = markings[(uint)mat2node.forward(i)];
        std::cout << "( ";
        for (uint j = 0; j < mk->size(); j++)
        {
            std::cout << mk->nToken(j) << ' ';
        }
        std::cout << ") ";
        std::cout << " prob:" << solution(i) << std::endl;
    }
}

TEST(petrinet, srn_two_absoring_groups_decomp)
{
    SrnCreator ct;
    auto p_tan1 = ct.place();
    auto p_tan2 = ct.place();
    auto p_left1 = ct.place();
    auto p_left2 = ct.place();
    auto p_right1 = ct.place();
    auto p_right2 = ct.place();
    auto p_right3 = ct.place();
    ct.expTrans(1).iarc(p_tan1).oarc(p_tan2);
    ct.expTrans(2).iarc(p_tan1).oarc(p_left1);
    ct.expTrans(3).iarc(p_tan2).oarc(p_right1);

    ct.expTrans(1).iarc(p_left1).oarc(p_left2);
    ct.expTrans(2).iarc(p_left2).oarc(p_left1);

    ct.expTrans(1).iarc(p_right1).oarc(p_right2);
    ct.expTrans(2).iarc(p_right2).oarc(p_right3);
    ct.expTrans(3).iarc(p_right3).oarc(p_right1);
    auto srn = ct.create();

    Marking mk(srn.placeCount());
    mk.setToken(p_tan1, 1);

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 7);

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
    printSrnSol(rg.nodeMarkings, sol.matrix2node, sol.nTransient,
                sol.solution);
    Real prob = 0.0;
    for (uint i = sol.nTransient; i < rg.nodeMarkings.size(); i++)
    {
        prob += sol.solution(i);
    }
    ASSERT_NEAR(prob, 1.0, tol);
}

TEST(petrinet, srn_two_absoring_groups_decomp2)
{
    SrnCreator ct;
    auto p_tan1 = ct.place();
    auto p_tan2 = ct.place();
    auto p_left1 = ct.place();
    auto p_left2 = ct.place();
    auto p_right1 = ct.place();
    auto p_right2 = ct.place();
    auto p_right3 = ct.place();
    ct.expTrans([](auto state) { return 1.0; }).iarc(p_tan1).oarc(p_tan2);
    ct.expTrans([](PetriNetState state) { return 2.0; })
        .iarc(p_tan1, [](auto state) { return 1; })
        .oarc(p_left1);
    ct.expTrans([](PetriNetState state) { return 3; })
        .iarc(p_tan2)
        .oarc(p_right1)
        .enable([](PetriNetState state) { return true; });

    ct.expTrans(1).iarc(p_left1).oarc(p_left2);
    ct.expTrans(2).iarc(p_left2).oarc(p_left1);

    ct.expTrans(1).iarc(p_right1).oarc(p_right2);
    ct.expTrans(2).iarc(p_right2).oarc(p_right3);
    ct.expTrans(3).iarc(p_right3).oarc(p_right1);
    auto srn = ct.create();

    Marking mk(srn.placeCount());
    mk.setToken(p_tan1, 1);

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 7);

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
    printSrnSol(rg.nodeMarkings, sol.matrix2node, sol.nTransient,
                sol.solution);
    Real prob = 0.0;
    for (uint i = sol.nTransient; i < rg.nodeMarkings.size(); i++)
    {
        prob += sol.solution(i);
    }
    ASSERT_NEAR(prob, 1.0, tol);
}

TEST(petrinet, srn_trivial_irreducible_bit)
{
    SrnCreator ct;
    auto p0 = ct.place(1);
    auto p1 = ct.place();
    ct.expTrans(1.0).iarc(p0).oarc(p1);
    ct.expTrans(2.0).iarc(p1).oarc(p0);
    auto srn = ct.create();
    auto mk = ct.bitMarking();

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 2);

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
    printSrnSol(rg.nodeMarkings, sol.matrix2node, sol.nTransient,
                sol.solution);
    Real prob = 0.0;
    for (uint i = sol.nTransient; i < rg.nodeMarkings.size(); i++)
    {
        prob += sol.solution(i);
        if (rg.nodeMarkings[(uint)sol.matrix2node.forward(i)]->nToken(0) == 1)
        {
            ASSERT_NEAR(sol.solution(i), 0.666, 0.001);
        }
        else
        {
            ASSERT_NEAR(sol.solution(i), 0.333, 0.001);
        }
    }
    ASSERT_NEAR(prob, 1.0, tol);
}

TEST(petrinet, srn_molloy_thesis)
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
    auto mk = ct.byteMarking();

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 5);

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

    printSrnSol(rg.nodeMarkings, sol.matrix2node, sol.nTransient,
                sol.solution);

    auto mk_p0 = srnProbReward(
        srn, sol, rg.nodeMarkings,
        [p0](PetriNetState state) { return srnPlaceToken(state, p0); });
    auto mk_p1 =
        srnProbReward(srn, sol, rg.nodeMarkings, srnPlaceTokenFunc(p1));
    auto mk_p3 =
        srnProbReward(srn, sol, rg.nodeMarkings, srnPlaceTokenFunc(p3));
    auto rate_t0 = srnProbReward(
        srn, sol, rg.nodeMarkings,
        [t0](PetriNetState state) { return srnTransRate(state, t0); });
    auto rate_t3 =
        srnProbReward(srn, sol, rg.nodeMarkings, srnTransRateFunc(t3));
    ASSERT_NEAR(mk_p0, 0.4497, 0.0001);
    ASSERT_NEAR(mk_p1, 0.4502, 0.0001);
    ASSERT_NEAR(mk_p3, 0.1, 0.0001);
    ASSERT_NEAR(rate_t0, 0.4497, 0.0001);
    ASSERT_NEAR(rate_t3, 0.9008, 0.0001);
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
TEST(petrinet, srn_software_mtta)
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

    auto srn = ct.create();
    auto mk = ct.byteMarking();

    auto rg = genReducedReachGraph(srn, mk);
    ASSERT_EQ(rg.graph.nodeCount(), 140);

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

    auto mtta = srnMtta(srn, sol, rg.nodeMarkings);
    std::cout << "mtta: " << mtta << std::endl;
    ASSERT_NEAR(mtta, 17.67, 0.01);
}

TEST(petrinet, srn_ts_acyclic1)
{
    SrnCreator ct;
    uint p0 = ct.place(2);
    ct.expTrans([p0](PetriNetState st) {
          return (Real)st.marking->nToken(p0) * 2.0;
      })
        .iarc(p0);
    auto srn = ct.create();
    auto mk = ct.marking();
    auto rg = genReducedReachGraph(srn, mk);
    {
        auto sol =
            srnTransientProb(rg.graph, rg.edgeRates, rg.initProbs, 0.0);
        ASSERT_EQ(sol.size(), 3);
        ASSERT_EQ(sol(0), 1.0);
        ASSERT_EQ(sol(1), 0.0);
        ASSERT_EQ(sol(2), 0.0);
    }
    {
        auto sol =
            srnTransientProb(rg.graph, rg.edgeRates, rg.initProbs, 1.0);
        ASSERT_EQ(sol.size(), 3);
        ASSERT_NEAR(sol(0), std::exp(-4.0 * 1.0), 1e-6);
        ASSERT_GT(sol(1), 0.0);
        ASSERT_GT(sol(2), 0.0);
    }
    {
        auto sol =
            srnTransientProb(rg.graph, rg.edgeRates, rg.initProbs, 100.0);
        ASSERT_EQ(sol.size(), 3);
        ASSERT_NEAR(sol(0), 0.0, 1e-6);
        ASSERT_NEAR(sol(1), 0.0, 1e-6);
        ASSERT_NEAR(sol(2), 1.0, 1e-6);
    }
}
