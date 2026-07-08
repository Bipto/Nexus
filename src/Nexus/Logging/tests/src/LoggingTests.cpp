#include <gtest/gtest.h>

#include "Logging/Logger.hpp"

#include <memory>

class TestLogSink : public Nexus::Logging::ILogSink
{
  public:
    ~TestLogSink() final = default;
    void LogMessage(const std::string &message) final
    {
    }
};

TEST(LoggingTest, TempTest)
{
    Nexus::Logging::Logger logger = {};
    logger.RegisterSink(std::make_unique<TestLogSink>());
    EXPECT_EQ(0, 0);
}