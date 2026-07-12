#include <gtest/gtest.h>

#include "Logging/Logger.hpp"

#include <memory>

class TestLogSink : public Nexus::Logging::ILogSink
{
  public:
    struct LogRecord
    {
        Nexus::Logging::LogLevel Level = {};
        std::string Message = {};
    };

  public:
    ~TestLogSink() final = default;

    void LogMessage(Nexus::Logging::LogLevel level, const std::string &message) final
    {
        LogRecords.emplace_back(level, message);
    }

    std::vector<LogRecord> LogRecords = {};
};

TEST(Logging, CreateRemoveSink)
{
    Nexus::Logging::Logger logger = {};

    // create sink
    Nexus::Logging::ILogSink *sink = logger.RegisterSink<TestLogSink>();
    EXPECT_EQ(logger.GetSinkCount(), 1);

    // remove sink
    logger.RemoveSink(sink);
    EXPECT_EQ(logger.GetSinkCount(), 0);
}

TEST(Logging, TestLogMessage)
{
    Nexus::Logging::Logger logger = {};

    // create sink
    TestLogSink *sink = logger.RegisterSink<TestLogSink>();
    EXPECT_EQ(logger.GetSinkCount(), 1);

    logger.Log(Nexus::Logging::LogLevel::Info, "This is a test log message");
    EXPECT_EQ(sink->LogRecords[0].Level, Nexus::Logging::LogLevel::Info);
    EXPECT_EQ(sink->LogRecords[0].Message, "This is a test log message");
}