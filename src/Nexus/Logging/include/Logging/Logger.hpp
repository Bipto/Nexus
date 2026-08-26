#pragma once

#include <format>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Nexus::Logging
{
    enum class LogLevel
    {
        Trace,
        Info,
        Warning,
        Error
    };

    class ILogSink
    {
      public:
        virtual ~ILogSink() = default;
        virtual void LogMessage(LogLevel level, const std::string &message) = 0;
    };

    class Logger
    {
      public:
        template <typename T, typename... Args>
        T *RegisterSink(Args &&...args)
        {
            static_assert(std::is_base_of_v<ILogSink, T>, "T must derive from ILogSink");

            auto sink = std::make_unique<T>(std::forward<Args>(args)...);
            T *ptr = sink.get();
            m_Sinks.emplace_back(std::move(sink));
            return ptr;
        }

        void RemoveSink(ILogSink *sink);
        size_t GetSinkCount() const;

        template <typename... Args>
        void Trace(std::format_string<Args...> fmt, Args &&...args)
        {
            Log(LogLevel::Trace, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Info(std::format_string<Args...> fmt, Args &&...args)
        {
            Log(LogLevel::Info, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Warning(std::format_string<Args...> fmt, Args &&...args)
        {
            Log(LogLevel::Warning, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Error(std::format_string<Args...> fmt, Args &&...args)
        {
            Log(LogLevel::Error, fmt, std::forward<Args>(args)...);
        }

      private:
        template <typename... Args>
        void Log(LogLevel level, std::format_string<Args...> fmt, Args &&...args)
        {
            auto message = std::format(fmt, std::forward<Args>(args)...);

            for (auto &logSink : m_Sinks)
            {
                logSink->LogMessage(level, message);
            }
        }

      private:
        std::vector<std::unique_ptr<ILogSink>> m_Sinks = {};
    };
} // namespace Nexus::Logging