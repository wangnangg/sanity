#include "gtest/gtest.h"

char **global_argv;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    global_argv = argv;
    return RUN_ALL_TESTS();
}
