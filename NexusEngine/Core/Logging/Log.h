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
            Log(const std::string& message, Severity severity);
    };

    class Logger
    {
        public:
            void LogInfo(std::string message);
            void LogWarning(std::string message);
            void LogError(std::string message);
            const std::vector<Nexus::Log>& GetLogs();
        private:
            static std::string GetTime();
        private:
            std::vector<Nexus::Log> m_Logs;
        
    };

    Logger* GetCoreLogger();
}

#define NX_LOG(message) Nexus::GetCoreLogger()->LogInfo(message)
#define NX_WARNING(message) Nexus::GetCoreLogger()->LogWarning(message)
#define NX_ERROR(message) Nexus::GetCoreLogger()->LogError(message)