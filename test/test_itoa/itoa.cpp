#include "../test.h"
#include <itoa.c>

//
// itoa
//
TEST(itoa, Radix10)
{
  char buffer[10];
  itoa(123, buffer, 10);
  EXPECT_STREQ(buffer, "123");
}

TEST(itoa, Radix10Negative)
{
  char buffer[10];
  itoa(-123, buffer, 10);
  EXPECT_STREQ(buffer, "-123");
}

TEST(itoa, Radix2)
{
  char buffer[10];
  itoa(123, buffer, 2);
  EXPECT_STREQ(buffer, "1111011");
}

TEST(itoa, Radix16)
{
  char buffer[10];
  itoa(123, buffer, 16);
  EXPECT_STREQ(buffer, "7b");
}

//
// ltoa
//
TEST(ltoa, Radix10)
{
  char buffer[10];
  ltoa(123, buffer, 10);
  EXPECT_STREQ(buffer, "123");
}

TEST(ltoa, Radix10Negative)
{
  char buffer[10];
  ltoa(-123, buffer, 10);
  EXPECT_STREQ(buffer, "-123");
}

TEST(ltoa, Radix2)
{
  char buffer[10];
  ltoa(123, buffer, 2);
  EXPECT_STREQ(buffer, "1111011");
}

TEST(ltoa, Radix16)
{
  char buffer[10];
  ltoa(123, buffer, 16);
  EXPECT_STREQ(buffer, "7b");
}

//
// utoa
//
TEST(utoa, Radix10)
{
  char buffer[10];
  utoa(123, buffer, 10);
  EXPECT_STREQ(buffer, "123");
}

TEST(utoa, Radix2)
{
  char buffer[10];
  utoa(123, buffer, 2);
  EXPECT_STREQ(buffer, "1111011");
}

TEST(utoa, Radix16)
{
  char buffer[10];
  utoa(123, buffer, 16);
  EXPECT_STREQ(buffer, "7b");
}

//
// ultoa
//
TEST(ultoa, Radix10)
{
  char buffer[10];
  ultoa(123, buffer, 10);
  EXPECT_STREQ(buffer, "123");
}

TEST(ultoa, Radix2)
{
  char buffer[10];
  ultoa(123, buffer, 2);
  EXPECT_STREQ(buffer, "1111011");
}

TEST(ultoa, Radix16)
{
  char buffer[10];
  ultoa(123, buffer, 16);
  EXPECT_STREQ(buffer, "7b");
}
