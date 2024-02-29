#include "../test.h"

TEST(SampleTestSuite, SampleTestCase) {
    int actual = 1;
    EXPECT_GT(actual, 0);
    EXPECT_EQ(1, actual) << "Should be equal to one";
}
