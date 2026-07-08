#include <filesystem>
#include <fstream>
#include <memory>

#include <gtest/gtest.h>

#include "Nexus-Core/Resources/MemoryResourceLoader.hpp"
#include "Nexus-Core/Shaders/ShaderPreprocessor.hpp"

class ShaderPreprocessorTests : public ::testing::Test
{
  protected:
    Nexus::MemoryResourceLoader loader;
    std::unique_ptr<Nexus::ShaderPreprocessor> preprocessor;

    void SetUp() override
    {
        preprocessor = std::make_unique<Nexus::ShaderPreprocessor>(&loader);
    }

    // Helper to mount a text file
    void MountText(std::string_view path, std::string_view text)
    {
        std::vector<std::byte> bytes(text.size());
        std::memcpy(bytes.data(), text.data(), text.size());
        ASSERT_TRUE(loader.MountBinaryFile(path, true, bytes).has_value());
    }
};

TEST_F(ShaderPreprocessorTests, NoIncludes_ReturnsOriginal)
{
    std::string shader = "void main() {}";

    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, shader);
}

TEST_F(ShaderPreprocessorTests, SingleInclude_ReplacedWithContent)
{
    MountText("common.glsl", "vec3 foo;");

    std::string shader = "#include \"common.glsl\"\n"
                         "void main() {}";

    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(
        result.value(), "vec3 foo;\n"
                        "void main() {}"
    );
}

TEST_F(ShaderPreprocessorTests, MissingInclude_ReturnsError)
{
    std::string shader = "#include \"missing.glsl\"";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().empty());
}

TEST_F(ShaderPreprocessorTests, NestedIncludes_ExpandCorrectly)
{
    MountText("a.glsl", "#include \"b.glsl\"");
    MountText("b.glsl", "float x;");

    std::string shader = "#include \"a.glsl\"";

    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "float x;");
}

TEST_F(ShaderPreprocessorTests, PreservesTrailingNewline)
{
    std::string shader = "void main() {}\n";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, shader);
}

TEST_F(ShaderPreprocessorTests, IncludeWithTrailingNewline)
{
    MountText("a.glsl", "float x;\n");

    std::string shader = "#include \"a.glsl\"";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "float x;\n");
}

TEST_F(ShaderPreprocessorTests, IncludeWithTrailingCode)
{
    MountText("common.glsl", "vec3 foo;");

    std::string shader = "#include \"common.glsl\" float bar;";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "vec3 foo; float bar;");
}

TEST_F(ShaderPreprocessorTests, IncludeIndented)
{
    MountText("common.glsl", "vec3 foo;");

    std::string shader = "    #include \"common.glsl\"";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "vec3 foo;");
}

TEST_F(ShaderPreprocessorTests, MultipleIncludes)
{
    MountText("a.glsl", "float a;");
    MountText("b.glsl", "float b;");

    std::string shader = "#include \"a.glsl\"\n"
                         "#include \"b.glsl\"";

    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "float a;\nfloat b;");
}

TEST_F(ShaderPreprocessorTests, DeepNestedIncludes)
{
    MountText("a.glsl", "#include \"b.glsl\"");
    MountText("b.glsl", "#include \"c.glsl\"");
    MountText("c.glsl", "int x;");

    std::string shader = "#include \"a.glsl\"";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "int x;");
}

TEST_F(ShaderPreprocessorTests, IncludeAngleBrackets)
{
    MountText("common.glsl", "vec3 foo;");

    std::string shader = "#include <common.glsl>";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "vec3 foo;");
}

TEST_F(ShaderPreprocessorTests, IncludeWithExtraSpaces)
{
    MountText("common.glsl", "vec3 foo;");

    std::string shader = "#include    \"common.glsl\"";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "vec3 foo;");
}

TEST_F(ShaderPreprocessorTests, IncludeFromIncludeDirectory)
{
    MountText("shaders/common.glsl", "vec3 foo;");

    std::string shader = "#include    \"common.glsl\"";
    std::vector<std::string> includeDirs = {"shaders"};

    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "vec3 foo;");
}

TEST_F(ShaderPreprocessorTests, EmptyInclude)
{
    MountText("empty.glsl", "");

    std::string shader = "#include \"empty.glsl\"";
    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "");
}

TEST_F(ShaderPreprocessorTests, IncludeBetweenCode)
{
    MountText("common.glsl", "vec3 foo;");

    std::string shader = "float a;\n"
                         "#include \"common.glsl\"\n"
                         "float b;";

    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(
        result.value(), "float a;\n"
                        "vec3 foo;\n"
                        "float b;"
    );
}

TEST_F(ShaderPreprocessorTests, PragmaOncePreventsDuplicateIncludes)
{
    MountText(
        "common.glsl", "#pragma once\n"
                       "float x;\n"
    );

    std::string shader = "#include \"common.glsl\"\n"
                         "#include \"common.glsl\"\n"
                         "void main() {}";

    std::vector<std::string> includeDirs = {};
    auto result = preprocessor->PreprocessShader("", shader, includeDirs);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(
        result.value(), "float x;\n"
                        "void main() {}"
    );
}
