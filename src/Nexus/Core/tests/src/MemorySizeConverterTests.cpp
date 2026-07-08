#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include "Core/MemorySize.hpp"

using namespace Nexus;

// Helper for floating-point comparisons
constexpr double EPS = 1e-9;

TEST(MemorySizeConverterTests, BytesToKilobytes)
{
    double result = MemorySizeConverter::Convert(1024, MemoryUnit::Bytes, MemoryUnit::Kilobytes);
    EXPECT_NEAR(result, 1.024, EPS);
}

TEST(MemorySizeConverterTests, KibibytesToBytes)
{
    double result = MemorySizeConverter::Convert(1, MemoryUnit::Kibibytes, MemoryUnit::Bytes);
    EXPECT_NEAR(result, 1024.0, EPS);
}

TEST(MemorySizeTests, ConstructAndConvert)
{
    MemorySize size(1, MemoryUnit::Megabytes);
    EXPECT_NEAR(size.To(MemoryUnit::Bytes), 1'000'000.0, EPS);
}

TEST(MemorySizeTests, Addition)
{
    MemorySize a(1, MemoryUnit::Megabytes);
    MemorySize b(500, MemoryUnit::Kilobytes);

    MemorySize result = a + b;

    EXPECT_NEAR(result.To(MemoryUnit::Bytes), 1'500'000.0, EPS);
}

TEST(MemorySizeTests, Subtraction)
{
    MemorySize a(2, MemoryUnit::Megabytes);
    MemorySize b(500, MemoryUnit::Kilobytes);

    MemorySize result = a - b;

    EXPECT_NEAR(result.To(MemoryUnit::Bytes), 1'500'000.0, EPS);
}

TEST(MemorySizeTests, Comparison)
{
    MemorySize a(1, MemoryUnit::Gigabytes);
    MemorySize b(1000, MemoryUnit::Megabytes);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);
}

TEST(MemorySizeTests, DefaultConstructorIsZero)
{
    MemorySize s;
    EXPECT_NEAR(s.To(MemoryUnit::Bytes), 0.0, EPS);
}

TEST(MemorySizeTests, NegativeValues)
{
    MemorySize s(-1, MemoryUnit::Kilobytes);
    EXPECT_NEAR(s.To(MemoryUnit::Bytes), -1000.0, EPS);
}

TEST(MemorySizeConverterTests, RoundTripConversion)
{
    double original = 12345.678;
    double bytes = MemorySizeConverter::Convert(original, MemoryUnit::Megabytes, MemoryUnit::Bytes);
    double back = MemorySizeConverter::Convert(bytes, MemoryUnit::Bytes, MemoryUnit::Megabytes);

    EXPECT_NEAR(back, original, EPS);
}

struct ConversionCase
{
    double value;
    MemoryUnit from;
    MemoryUnit to;
    double expected;
};

class AllConversionTests : public ::testing::TestWithParam<ConversionCase>
{
};

static std::vector<ConversionCase> GetConversionCases()
{
    return {
        // Bytes <-> Kilobytes (SI)
        {1000.0, MemoryUnit::Bytes, MemoryUnit::Kilobytes, 1.0},
        {1.0, MemoryUnit::Kilobytes, MemoryUnit::Bytes, 1000.0},

        // Bytes <-> Kibibytes (IEC)
        {1024.0, MemoryUnit::Bytes, MemoryUnit::Kibibytes, 1.0},
        {1.0, MemoryUnit::Kibibytes, MemoryUnit::Bytes, 1024.0},

        // Kilobytes <-> Megabytes (SI)
        {1000.0, MemoryUnit::Kilobytes, MemoryUnit::Megabytes, 1.0},
        {1.0, MemoryUnit::Megabytes, MemoryUnit::Kilobytes, 1000.0},

        // Kibibytes <-> Mebibytes (IEC)
        {1024.0, MemoryUnit::Kibibytes, MemoryUnit::Mibibytes, 1.0},
        {1.0, MemoryUnit::Mibibytes, MemoryUnit::Kibibytes, 1024.0},

        // Megabytes <-> Gigabytes (SI)
        {1000.0, MemoryUnit::Megabytes, MemoryUnit::Gigabytes, 1.0},
        {1.0, MemoryUnit::Gigabytes, MemoryUnit::Megabytes, 1000.0},

        // Mebibytes <-> Gibibytes (IEC)
        {1024.0, MemoryUnit::Mibibytes, MemoryUnit::Gibibytes, 1.0},
        {1.0, MemoryUnit::Gibibytes, MemoryUnit::Mibibytes, 1024.0},

        // …and so on for TB, TiB, PB, PiB
    };
}

INSTANTIATE_TEST_SUITE_P(AllConversions, AllConversionTests, ::testing::ValuesIn(GetConversionCases()));

TEST_P(AllConversionTests, ConvertsCorrectly)
{
    auto p = GetParam();
    EXPECT_NEAR(MemorySizeConverter::Convert(p.value, p.from, p.to), p.expected, EPS);
}

TEST(MemorySizeTests, AddZero)
{
    MemorySize a(1, MemoryUnit::Gigabytes);
    MemorySize zero(0, MemoryUnit::Bytes);

    EXPECT_NEAR((a + zero).To(MemoryUnit::Gigabytes), 1.0, EPS);
}

TEST(MemorySizeTests, SubtractToZero)
{
    MemorySize a(1, MemoryUnit::Megabytes);
    MemorySize b(1, MemoryUnit::Megabytes);

    EXPECT_NEAR((a - b).To(MemoryUnit::Bytes), 0.0, EPS);
}

TEST(MemorySizeTests, LargeValues)
{
    MemorySize a(1e12, MemoryUnit::Bytes);
    EXPECT_NEAR(a.To(MemoryUnit::Gigabytes), 1e12 / 1e9, EPS);
}

TEST(MemorySizeTests, LessThanComparison)
{
    MemorySize a(1, MemoryUnit::Kilobytes);
    MemorySize b(2, MemoryUnit::Kilobytes);

    EXPECT_TRUE(a < b);
}

TEST(MemorySizeTests, CrossUnitComparison)
{
    MemorySize a(1024, MemoryUnit::Bytes);
    MemorySize b(1, MemoryUnit::Kibibytes);

    EXPECT_TRUE(a == b);
}

TEST(MemorySizeTests, ThreeWayOperator)
{
    MemorySize a(1, MemoryUnit::Megabytes);
    MemorySize b(2, MemoryUnit::Megabytes);

    auto cmp = (a <=> b);
    EXPECT_TRUE(cmp < 0);
    EXPECT_FALSE(cmp > 0);
    EXPECT_FALSE(cmp == 0);
}

TEST(MemorySizeTests, InternalNormalization)
{
    MemorySize a(1, MemoryUnit::Kilobytes);
    EXPECT_NEAR(a.To(MemoryUnit::Bytes), 1000.0, EPS);
}
