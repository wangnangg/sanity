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
    auto p_live = ct.place(3);
    auto p_dead = ct.place();
    ct.expTrans(1.0).iarc(p_live).oarc(p_dead);
    ct.expTrans([p_dead](PetriNetState st) {
          return st.marking->nToken(p_dead) * 1.0;
      })
        .iarc(p_dead)
        .oarc(p_live);
    auto srn = ct.create();
    auto mk = ct.marking();
    auto rg = genReducedReachGraph(srn, mk);
    auto Q = srnRateMatrix(rg.graph, rg.edgeRates);
    auto prob = Vector(rg.nodeMarkings.size(), 1.0);
    auto iter = srnSteadyStateSor(Q, mutableView(prob), 1.0, 1e-6, 1000);
    std::cout << Q << std::endl;
    std::cout << prob << std::endl;
    std::cout << "sor nIter: " << iter.nIter << ", error: " << iter.error
              << std::endl;
    ASSERT_LT(iter.error, 1e-6);
}
