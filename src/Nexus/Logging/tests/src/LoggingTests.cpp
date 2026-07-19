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

    logger.Trace("This is a test trace message");
    EXPECT_EQ(sink->LogRecords[0].Level, Nexus::Logging::LogLevel::Trace);
    EXPECT_EQ(sink->LogRecords[0].Message, "This is a test trace message");

    logger.Info("This is a test info message");
    EXPECT_EQ(sink->LogRecords[1].Level, Nexus::Logging::LogLevel::Info);
    EXPECT_EQ(sink->LogRecords[1].Message, "This is a test info message");

    logger.Warning("This is a test warning message");
    EXPECT_EQ(sink->LogRecords[2].Level, Nexus::Logging::LogLevel::Warning);
    EXPECT_EQ(sink->LogRecords[2].Message, "This is a test warning message");

    logger.Error("This is a test error message");
    EXPECT_EQ(sink->LogRecords[3].Level, Nexus::Logging::LogLevel::Error);
    EXPECT_EQ(sink->LogRecords[3].Message, "This is a test error message");
}