#include "Log.h"

#include <time.h>

Nexus::Logger* logger = new Nexus::Logger();

namespace Nexus
{
    Log::Log(const std::string& message, Severity severity)
    {
        Message = message;
        MessageSeverity = severity;
        Time = std::chrono::system_clock::now();
    }

    void Logger::LogInfo(std::string message)
    {
        Nexus::Log log("[INFO] received at " + Logger::GetTime() + std::string(":\n") + message, Severity::Info);
        std::cout << log.Message << std::endl;
        this->m_Logs.push_back(log);
    }

    void Logger::LogWarning(std::string message)
    {
        Nexus::Log log("[WARNING] received at " + Logger::GetTime() + std::string(":\n") + message, Severity::Info);
        std::cout << log.Message << std::endl;
        this->m_Logs.push_back(log);
    }

    void Logger::LogError(std::string message)
    {
        Nexus::Log log("[ERROR] received at " + Logger::GetTime() + std::string(":\n") + message, Severity::Info);
        std::cout << log.Message << std::endl;
        this->m_Logs.push_back(log);
    }

    const std::vector<Nexus::Log>& Logger::GetLogs()
    {
        return m_Logs;
    }

    std::string Logger::GetTime()
    {
        std::time_t time = std::time(nullptr);
        struct tm tstruct;
        char buff[80];
        tstruct = *std::localtime(&time);
        strftime(buff, sizeof(buff), "%d-%m-%Y %X", &tstruct);
        return buff;
    }

    Logger* GetCoreLogger()
    {
        return logger;
    }
}