#pragma once

#include "Core/nxpch.h"

namespace Nexus
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
            void LogInfo(std::string message)
            {
                Nexus::Log log("INFO: " + message, Severity::Info);
                std::cout << log.Message << std::endl;
                this->m_Logs.push_back(log);
            }

            void LogWarning(std::string message)
            {
                Nexus::Log log("WARNING: " + message, Severity::Warning);
                std::cout << log.Message << std::endl;
                this->m_Logs.push_back(log);
            }

            void LogError(std::string message)
            {
                Nexus::Log log("ERROR: " + message, Severity::Error);
                std::cout << log.Message << std::endl;
                this->m_Logs.push_back(log);
            }

            const std::vector<Nexus::Log>& GetLogs()
            {
                return m_Logs;
            }
        private:
            std::vector<Nexus::Log> m_Logs;
    };
}