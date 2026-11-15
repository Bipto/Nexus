#include <gtest/gtest.h>

#include "Maths/Vector2.hpp"

#include <cstdint>

using namespace Nexus::Maths;

// Types used for general Vector2 tests.
using VectorTypes = ::testing::Types<int, float, double>;

// Types used for integral-only operations.
using IntegerTypes = ::testing::Types<int, std::int32_t, std::int64_t>;

template <typename T>
class Vector2Test : public ::testing::Test
{
};

template <typename T>
class Vector2IntegerTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(Vector2Test, VectorTypes);
TYPED_TEST_SUITE(Vector2IntegerTest, IntegerTypes);

// =============================================
// Constructor Tests
// =============================================

TYPED_TEST(Vector2Test, ConstructorInitialisesComponents)
{
    using T = TypeParam;

    Vector2<T> v(T{3}, T{4});

    EXPECT_EQ(v.X, T{3});
    EXPECT_EQ(v.Y, T{4});
}

TYPED_TEST(Vector2Test, ConstructorSupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> v(T{-3}, T{-4});

    EXPECT_EQ(v.X, T{-3});
    EXPECT_EQ(v.Y, T{-4});
}

TYPED_TEST(Vector2Test, ConstructorSupportsZeroValues)
{
    using T = TypeParam;

    Vector2<T> v(T{0}, T{0});

    EXPECT_EQ(v.X, T{0});
    EXPECT_EQ(v.Y, T{0});
}

// =============================================
// Addition Tests
// =============================================

TYPED_TEST(Vector2Test, AddReturnsCorrectResult)
{
    using T = TypeParam;

    Vector2<T> a(T{2}, T{3});
    Vector2<T> b(T{4}, T{5});

    auto result = Vector2<T>::Add(a, b);

    EXPECT_EQ(result.X, T{6});
    EXPECT_EQ(result.Y, T{8});
}

TYPED_TEST(Vector2Test, AddSupportsZeroVector)
{
    using T = TypeParam;

    Vector2<T> a(T{2}, T{3});
    Vector2<T> zero(T{0}, T{0});

    auto result = Vector2<T>::Add(a, zero);

    EXPECT_EQ(result.X, a.X);
    EXPECT_EQ(result.Y, a.Y);
}

TYPED_TEST(Vector2Test, AddSupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> a(T{5}, T{-3});
    Vector2<T> b(T{-2}, T{7});

    auto result = Vector2<T>::Add(a, b);

    EXPECT_EQ(result.X, T{3});
    EXPECT_EQ(result.Y, T{4});
}

// =============================================
// Subtraction Tests
// =============================================

TYPED_TEST(Vector2Test, SubtractReturnsCorrectResult)
{
    using T = TypeParam;

    Vector2<T> a(T{8}, T{7});
    Vector2<T> b(T{3}, T{2});

    auto result = Vector2<T>::Subtract(a, b);

    EXPECT_EQ(result.X, T{5});
    EXPECT_EQ(result.Y, T{5});
}

TYPED_TEST(Vector2Test, SubtractVectorFromItselfReturnsZero)
{
    using T = TypeParam;

    Vector2<T> v(T{5}, T{8});

    auto result = Vector2<T>::Subtract(v, v);

    EXPECT_EQ(result.X, T{0});
    EXPECT_EQ(result.Y, T{0});
}

TYPED_TEST(Vector2Test, SubtractSupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> a(T{3}, T{-5});
    Vector2<T> b(T{7}, T{-2});

    auto result = Vector2<T>::Subtract(a, b);

    EXPECT_EQ(result.X, T{-4});
    EXPECT_EQ(result.Y, T{-3});
}

// =============================================
// Multiplication Tests
// =============================================

TYPED_TEST(Vector2Test, MultiplyReturnsCorrectResult)
{
    using T = TypeParam;

    Vector2<T> a(T{2}, T{3});
    Vector2<T> b(T{4}, T{5});

    auto result = Vector2<T>::Multiply(a, b);

    EXPECT_EQ(result.X, T{8});
    EXPECT_EQ(result.Y, T{15});
}

TYPED_TEST(Vector2Test, MultiplySupportsZeroVector)
{
    using T = TypeParam;

    Vector2<T> a(T{2}, T{3});
    Vector2<T> zero(T{0}, T{0});

    auto result = Vector2<T>::Multiply(a, zero);

    EXPECT_EQ(result.X, T{0});
    EXPECT_EQ(result.Y, T{0});
}

TYPED_TEST(Vector2Test, MultiplySupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> a(T{-2}, T{3});
    Vector2<T> b(T{4}, T{-5});

    auto result = Vector2<T>::Multiply(a, b);

    EXPECT_EQ(result.X, T{-8});
    EXPECT_EQ(result.Y, T{-15});
}

// =============================================
// Division Tests
// =============================================

TYPED_TEST(Vector2Test, DivideReturnsCorrectResult)
{
    using T = TypeParam;

    Vector2<T> a(T{12}, T{20});
    Vector2<T> b(T{3}, T{4});

    auto result = Vector2<T>::Divide(a, b);

    EXPECT_EQ(result.X, T{4});
    EXPECT_EQ(result.Y, T{5});
}

TYPED_TEST(Vector2Test, DivideSupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> a(T{-12}, T{20});
    Vector2<T> b(T{3}, T{-4});

    auto result = Vector2<T>::Divide(a, b);

    EXPECT_EQ(result.X, T{-4});
    EXPECT_EQ(result.Y, T{-5});
}

// =============================================
// Negation Tests
// =============================================

TYPED_TEST(Vector2Test, NegateReturnsCorrectResult)
{
    using T = TypeParam;

    Vector2<T> v(T{5}, T{8});

    auto result = Vector2<T>::Negate(v);

    EXPECT_EQ(result.X, T{-5});
    EXPECT_EQ(result.Y, T{-8});
}

TYPED_TEST(Vector2Test, NegateSupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> v(T{-5}, T{-8});

    auto result = Vector2<T>::Negate(v);

    EXPECT_EQ(result.X, T{5});
    EXPECT_EQ(result.Y, T{8});
}

TYPED_TEST(Vector2Test, NegateSupportsZeroVector)
{
    using T = TypeParam;

    Vector2<T> v(T{0}, T{0});

    auto result = Vector2<T>::Negate(v);

    EXPECT_EQ(result.X, T{0});
    EXPECT_EQ(result.Y, T{0});
}

// =============================================
// Modulus Tests (Integral Types Only)
// =============================================

TYPED_TEST(Vector2IntegerTest, ModulusReturnsCorrectResult)
{
    using T = TypeParam;

    Vector2<T> a(T{17}, T{23});
    Vector2<T> b(T{5}, T{7});

    auto result = Vector2<T>::Modulus(a, b);

    EXPECT_EQ(result.X, T{2});
    EXPECT_EQ(result.Y, T{2});
}

TYPED_TEST(Vector2IntegerTest, ModulusReturnsZeroForExactDivision)
{
    using T = TypeParam;

    Vector2<T> a(T{12}, T{20});
    Vector2<T> b(T{3}, T{4});

    auto result = Vector2<T>::Modulus(a, b);

    EXPECT_EQ(result.X, T{0});
    EXPECT_EQ(result.Y, T{0});
}

TYPED_TEST(Vector2IntegerTest, ModulusSupportsNegativeValues)
{
    using T = TypeParam;

    Vector2<T> a(T{-17}, T{23});
    Vector2<T> b(T{5}, T{-7});

    auto result = Vector2<T>::Modulus(a, b);

    EXPECT_EQ(result.X, T{-2});
    EXPECT_EQ(result.Y, T{2});
}

TYPED_TEST(Vector2IntegerTest, ModulusSupportsNegativeDivisors)
{
    using T = TypeParam;

    Vector2<T> a(T{17}, T{23});
    Vector2<T> b(T{-5}, T{-7});

    auto result = Vector2<T>::Modulus(a, b);

    EXPECT_EQ(result.X, T{2});
    EXPECT_EQ(result.Y, T{2});
}