#include "../test.h"
#include <avr/dtostrf.c>

TEST(dtostrf, PositiveNumber)
{
    char buffer[10];
    dtostrf(123.456, 6, 2, buffer);

    EXPECT_STREQ(buffer, "123.46");
}

TEST(dtostrf, NegativeNumber)
{
    char buffer[10];
    dtostrf(-123.456, 6, 2, buffer);

    EXPECT_STREQ(buffer, "-123.46");
}

TEST(dtostrf, Zero)
{
    char buffer[10];
    dtostrf(0, 3, 2, buffer);

    EXPECT_STREQ(buffer, "0.00");
}

TEST(dtostrf, Padding)
{
    char buffer[10];
    dtostrf(123.456, 10, 2, buffer);

    EXPECT_STREQ(buffer, "    123.46");
}

TEST(dtostrf, ZeroPrecision)
{
    char buffer[10];
    dtostrf(123.456, 3, 0, buffer);

    EXPECT_STREQ(buffer, "123");
}
