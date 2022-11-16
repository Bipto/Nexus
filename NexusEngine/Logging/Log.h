#pragma once

#include "../Core/nxpch.h"

namespace NexusEngine
{
    enum class Severity
    {
        Info,
        Warning,
        Error
    };

    struct Log
    {
        public:
            std::string Message;
            Severity MessageSeverity;
            std::chrono::system_clock::time_point Time;
        public:
            Log(const std::string& message, Severity severity)
            {
                Message = message;
                MessageSeverity = severity;
                Time = std::chrono::system_clock::now();
            }
    };

    class Logger
    {
        public:
            void LogMessage(std::string message, Severity messageSeverity)
            {
                NexusEngine::Log log(message, messageSeverity);
                std::cout << log.Message << std::endl;
                m_Logs.push_back(log);
            }
        private:
            std::vector<NexusEngine::Log> m_Logs;
    };
}

//#define HZ_LOG(message, severity) ::NexusEngine::Logging::GetCoreLogger()->Log(message, severity)