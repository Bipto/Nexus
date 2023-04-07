#include "Log.h"

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
        Nexus::Log log("INFO: " + message, Severity::Info);
        std::cout << log.Message << std::endl;
        this->m_Logs.push_back(log);
    }

    void Logger::LogWarning(std::string message)
    {
        Nexus::Log log("WARNING: " + message, Severity::Warning);
        std::cout << log.Message << std::endl;
        this->m_Logs.push_back(log);
    }

    void Logger::LogError(std::string message)
    {
        Nexus::Log log("ERROR: " + message, Severity::Error);
        std::cout << log.Message << std::endl;
        this->m_Logs.push_back(log);
    }

    const std::vector<Nexus::Log>& Logger::GetLogs()
    {
        return m_Logs;
    }

    Logger* GetCoreLogger()
    {
        return logger;
    }
}