#include "../test.h"
#include <Arduino.h>

TEST(Arduino, min)
{
    EXPECT_EQ(1, min(1, 2));
    EXPECT_EQ(1, min(2, 1));
    EXPECT_EQ(1, min(1, 1));
}

TEST(Arduino, max)
{
    EXPECT_EQ(2, max(1, 2));
    EXPECT_EQ(2, max(2, 1));
    EXPECT_EQ(1, max(1, 1));
}

TEST(Arduino, abs)
{
    EXPECT_EQ(1, abs(1));
    EXPECT_EQ(1, abs(-1));
    EXPECT_EQ(0, abs(0));
}

TEST(Arduino, constrain)
{
    EXPECT_EQ(1, constrain(1, 0, 2));
    EXPECT_EQ(0, constrain(-1, 0, 2));
    EXPECT_EQ(2, constrain(3, 0, 2));
    EXPECT_EQ(0, constrain(-1, 0, 0));
}

TEST(Arduino, round)
{
    EXPECT_EQ(1, round(1.4));
    EXPECT_EQ(2, round(1.5));
    EXPECT_EQ(2, round(1.6));
    EXPECT_EQ(-1, round(-1.4));
    EXPECT_EQ(-2, round(-1.5));
    EXPECT_EQ(-2, round(-1.6));
}

TEST(Arduino, sq)
{
    EXPECT_EQ(1, sq(1));
    EXPECT_EQ(4, sq(2));
    EXPECT_EQ(0, sq(0));
    EXPECT_EQ(1, sq(-1));
    EXPECT_EQ(4, sq(-2));
}

TEST(Arduino, lowByte)
{
    EXPECT_EQ(0, lowByte(0x0000));
    EXPECT_EQ(0xFF, lowByte(0x00FF));
    EXPECT_EQ(0xFF, lowByte(0xFFFF));
}

TEST(Arduino, highByte)
{
    EXPECT_EQ(0, highByte(0x0000));
    EXPECT_EQ(0, highByte(0x00FF));
    EXPECT_EQ(0xFF, highByte(0xFFFF));
}

TEST(Arduino, bitRead)
{
    EXPECT_EQ(0, bitRead(0x0000, 0));
    EXPECT_EQ(1, bitRead(0x0001, 0));
    EXPECT_EQ(0, bitRead(0x0002, 0));
    EXPECT_EQ(1, bitRead(0x0002, 1));
}

TEST(Arduino, bitSet)
{
    uint16_t value = 0x0000;

    bitSet(value, 0);
    EXPECT_EQ(0x0001, value);

    bitSet(value, 1);
    EXPECT_EQ(0x0003, value);
}

TEST(Arduino, bitClear)
{
    uint16_t value = 0xFFFF;

    bitClear(value, 0);
    EXPECT_EQ(0xFFFE, value);

    bitClear(value, 1);
    EXPECT_EQ(0xFFFC, value);
}

TEST(Arduino, bitWrite)
{
    uint16_t value = 0x0000;

    bitWrite(value, 0, 1);
    EXPECT_EQ(0x0001, value);

    bitWrite(value, 1, 1);
    EXPECT_EQ(0x0003, value);

    bitWrite(value, 0, 0);
    EXPECT_EQ(0x0002, value);

    bitWrite(value, 1, 0);
    EXPECT_EQ(0x0000, value);
}

TEST(Arduino, bit)
{
    EXPECT_EQ(0x0001, bit(0));
    EXPECT_EQ(0x0002, bit(1));
    EXPECT_EQ(0x0004, bit(2));
    EXPECT_EQ(0x0008, bit(3));
    EXPECT_EQ(0x0010, bit(4));
    EXPECT_EQ(0x4000, bit(14));
    EXPECT_EQ(0x8000, bit(15));
}
