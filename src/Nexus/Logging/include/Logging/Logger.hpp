#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Nexus::Logging
{
    class ILogSink
    {
      public:
        virtual ~ILogSink() = default;
        virtual void LogMessage(const std::string &message) = 0;
    };

    class Logger
    {
      public:
        void RegisterSink(std::unique_ptr<ILogSink> sink);

      private:
        std::vector<std::unique_ptr<ILogSink>> m_Sinks = {};
    };
} // namespace Nexus::Logging