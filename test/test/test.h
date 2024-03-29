/**
 * common header for all unit tests for setting up the test environment
 */
#include <gtest/gtest.h>
#include <core_types.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
