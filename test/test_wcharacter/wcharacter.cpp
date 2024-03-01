#include "../test.h"
#include <WCharacter.h>

TEST(WCharacter, isAlphaNumeric)
{
    EXPECT_TRUE(isAlphaNumeric('a'));
    EXPECT_TRUE(isAlphaNumeric('z'));
    EXPECT_TRUE(isAlphaNumeric('A'));
    EXPECT_TRUE(isAlphaNumeric('Z'));
    EXPECT_TRUE(isAlphaNumeric('0'));
    EXPECT_TRUE(isAlphaNumeric('9'));
    EXPECT_FALSE(isAlphaNumeric(' '));
    EXPECT_FALSE(isAlphaNumeric('\n'));
    EXPECT_FALSE(isAlphaNumeric('\t'));
    EXPECT_FALSE(isAlphaNumeric('\0'));
}

TEST(WCharacter, isAlpha)
{
    EXPECT_TRUE(isAlpha('a'));
    EXPECT_TRUE(isAlpha('z'));
    EXPECT_TRUE(isAlpha('A'));
    EXPECT_TRUE(isAlpha('Z'));
    EXPECT_FALSE(isAlpha('0'));
    EXPECT_FALSE(isAlpha('9'));
    EXPECT_FALSE(isAlpha(' '));
    EXPECT_FALSE(isAlpha('\n'));
    EXPECT_FALSE(isAlpha('\t'));
    EXPECT_FALSE(isAlpha('\0'));
}

TEST(WCharacter, isAscii)
{
    for (int i = 0; i < 128; i++)
    {
        EXPECT_TRUE(isAscii(i));
    }
    for (int i = 128; i < 256; i++)
    {
        EXPECT_FALSE(isAscii(i));
    }
}

TEST(WCharacter, isWhitespace)
{
    EXPECT_TRUE(isWhitespace(' '));
    EXPECT_TRUE(isWhitespace('\t'));
    EXPECT_FALSE(isWhitespace('a'));
    EXPECT_FALSE(isWhitespace('0'));
    EXPECT_FALSE(isWhitespace('\n'));
    EXPECT_FALSE(isWhitespace('\0'));
}

TEST(WCharacter, isControl)
{
    EXPECT_TRUE(isControl('\0'));
    for (int i = 1; i < 32; i++)
    {
        EXPECT_TRUE(isControl(i));
    }

    for (int i = 32; i < 127; i++)
    {
        EXPECT_FALSE(isControl(i));
    }
}

TEST(WCharacter, isDigit)
{
    EXPECT_TRUE(isDigit('0'));
    EXPECT_TRUE(isDigit('9'));
    EXPECT_FALSE(isDigit('a'));
    EXPECT_FALSE(isDigit('A'));
    EXPECT_FALSE(isDigit(' '));
    EXPECT_FALSE(isDigit('\n'));
    EXPECT_FALSE(isDigit('\t'));
    EXPECT_FALSE(isDigit('\0'));
}

TEST(WCharacter, isGraph)
{
    for (int i = 33; i < 127; i++)
    {
        EXPECT_TRUE(isGraph(i));
    }
    for (int i = 0; i < 33; i++)
    {
        EXPECT_FALSE(isGraph(i));
    }
    for (int i = 127; i < 256; i++)
    {
        EXPECT_FALSE(isGraph(i));
    }
}

TEST(WCharacter, isLowerCase)
{
    EXPECT_TRUE(isLowerCase('a'));
    EXPECT_TRUE(isLowerCase('z'));
    EXPECT_FALSE(isLowerCase('A'));
    EXPECT_FALSE(isLowerCase('Z'));
    EXPECT_FALSE(isLowerCase('0'));
}

TEST(WCharacter, isPrintable)
{
    for (int i = 32; i < 127; i++)
    {
        EXPECT_TRUE(isPrintable(i));
    }
    for (int i = 0; i < 32; i++)
    {
        EXPECT_FALSE(isPrintable(i));
    }
}

TEST(WCharacter, isPunct)
{
    EXPECT_TRUE(isPunct('!'));
    EXPECT_TRUE(isPunct('"'));
    EXPECT_TRUE(isPunct('#'));
    EXPECT_TRUE(isPunct('$'));
    EXPECT_TRUE(isPunct('%'));
    EXPECT_TRUE(isPunct('&'));
    EXPECT_TRUE(isPunct('\''));
    EXPECT_TRUE(isPunct('('));
    EXPECT_TRUE(isPunct(')'));
    EXPECT_TRUE(isPunct('*'));
    EXPECT_TRUE(isPunct('+'));
    EXPECT_TRUE(isPunct(','));
    EXPECT_TRUE(isPunct('-'));
    EXPECT_TRUE(isPunct('.'));
    EXPECT_TRUE(isPunct('/'));
    EXPECT_TRUE(isPunct(':'));
    EXPECT_TRUE(isPunct(';'));
    EXPECT_TRUE(isPunct('<'));
    EXPECT_TRUE(isPunct('='));
    EXPECT_TRUE(isPunct('>'));
    EXPECT_TRUE(isPunct('?'));
    EXPECT_TRUE(isPunct('@'));
    EXPECT_TRUE(isPunct('['));
    EXPECT_TRUE(isPunct('\\'));
    EXPECT_TRUE(isPunct(']'));
    EXPECT_TRUE(isPunct('^'));
    EXPECT_TRUE(isPunct('_'));
    EXPECT_TRUE(isPunct('`'));
    EXPECT_TRUE(isPunct('{'));
    EXPECT_TRUE(isPunct('|'));
    EXPECT_TRUE(isPunct('}'));
    EXPECT_TRUE(isPunct('~'));
    EXPECT_FALSE(isPunct('a'));
    EXPECT_FALSE(isPunct('A'));
    EXPECT_FALSE(isPunct('0'));
    EXPECT_FALSE(isPunct(' '));
    EXPECT_FALSE(isPunct('\n'));
    EXPECT_FALSE(isPunct('\t'));
}

TEST(WCharacter, isSpace)
{
    EXPECT_TRUE(isSpace(' '));
    EXPECT_TRUE(isSpace('\t'));
    EXPECT_TRUE(isSpace('\n'));
    EXPECT_TRUE(isSpace('\v'));
    EXPECT_TRUE(isSpace('\f'));
    EXPECT_TRUE(isSpace('\r'));
    EXPECT_FALSE(isSpace('a'));
    EXPECT_FALSE(isSpace('0'));
    EXPECT_FALSE(isSpace('\0'));
}

TEST(WCharacter, isUpperCase)
{
    EXPECT_FALSE(isUpperCase('a'));
    EXPECT_FALSE(isUpperCase('z'));
    EXPECT_TRUE(isUpperCase('A'));
    EXPECT_TRUE(isUpperCase('Z'));
    EXPECT_FALSE(isUpperCase('0'));
}

TEST(WCharacter, toLowerCase)
{
    EXPECT_EQ(toLowerCase('A'), 'a');
    EXPECT_EQ(toLowerCase('Z'), 'z');

    for (int i = 'a'; i <= 'z'; i++)
    {
        EXPECT_EQ(toLowerCase(i), i);
    }
}

TEST(WCharacter, toUpperCase)
{
    EXPECT_EQ(toUpperCase('a'), 'A');
    EXPECT_EQ(toUpperCase('z'), 'Z');

    for (int i = 'A'; i <= 'Z'; i++)
    {
        EXPECT_EQ(toUpperCase(i), i);
    }
}

TEST(WCharacter, isHexadecimalDigit)
{
    for (int i = '0'; i <= '9'; i++)
    {
        EXPECT_TRUE(isHexadecimalDigit(i));
    }
    for (int i = 'A'; i <= 'F'; i++)
    {
        EXPECT_TRUE(isHexadecimalDigit(i));
    }
    for (int i = 'a'; i <= 'f'; i++)
    {
        EXPECT_TRUE(isHexadecimalDigit(i));
    }
    for (int i = 0; i < '0'; i++)
    {
        EXPECT_FALSE(isHexadecimalDigit(i));
    }
    for (int i = '9' + 1; i < 'A'; i++)
    {
        EXPECT_FALSE(isHexadecimalDigit(i));
    }
    for (int i = 'F' + 1; i < 'a'; i++)
    {
        EXPECT_FALSE(isHexadecimalDigit(i));
    }
    for (int i = 'f' + 1; i < 256; i++)
    {
        EXPECT_FALSE(isHexadecimalDigit(i));
    }
}

TEST(WCharacter, toAscii)
{
    for (int i = 0; i < 128; i++)
    {
        EXPECT_EQ(toAscii(i), i);
    }
    for (int i = 128; i < 256; i++)
    {
        EXPECT_EQ(toAscii(i), i - 128);
    }
}
