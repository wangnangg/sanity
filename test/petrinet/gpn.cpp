#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "petrinet.hpp"

using namespace sanity::petrinet;
using namespace sanity::simulate;
using namespace sanity::linear;
TEST(petrinet, gpn_create)
{
    GpnCreator creator;
    creator.place(1);
    creator.place(0);
    creator.expTrans(1.0).iarc(0).oarc(1);
    creator.expTrans(2.0).iarc(1).oarc(0);
    auto gpn = creator.create();
    ASSERT_EQ(gpn.transCount(), 2);
    ASSERT_EQ(gpn.placeCount(), 2);
    ASSERT_EQ(gpn.transProps.size(), 2);
}
