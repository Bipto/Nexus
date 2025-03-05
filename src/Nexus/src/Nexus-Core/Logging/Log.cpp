#include "Nexus-Core/Logging/Log.hpp"

#include "Nexus-Core/nxpch.hpp"

Nexus::Logger *logger = new Nexus::Logger();

namespace Nexus
{
	Log::Log(const std::string &message, Severity severity) : Message(message), MessageSeverity(severity), Time(std::chrono::system_clock::now())
	{
	}

	void Logger::LogInfo(const std::string &message, const std::source_location location)
	{
		// build the message
		std::stringstream ss;
		ss << "[INFO] received from file: {";
		ss << location.file_name();
		ss << "} at line: ";
		ss << location.line();
		ss << ": ";
		ss << message;

		Nexus::Log log(ss.str(), Severity::Info);
		std::cout << log.Message << std::endl;
		this->m_Logs.push_back(log);
	}

	void Logger::LogWarning(const std::string &message, const std::source_location location)
	{
		// build the message
		std::stringstream ss;
		ss << "[WARNING] received from file: {";
		ss << location.file_name();
		ss << "} at line: ";
		ss << location.line();
		ss << ": ";
		ss << message;

		Nexus::Log log(ss.str(), Severity::Warning);
		std::cout << log.Message << std::endl;
		this->m_Logs.push_back(log);
	}

	void Logger::LogError(const std::string &message, const std::source_location location)
	{
		// build the message
		std::stringstream ss;
		ss << "[ERROR] received from file: {";
		ss << location.file_name();
		ss << "} at line: ";
		ss << std::to_string(location.line());
		ss << ": ";
		ss << message;

		Nexus::Log log(ss.str(), Severity::Error);
		std::cout << log.Message << std::endl;
		this->m_Logs.push_back(log);
	}

	const std::vector<Nexus::Log> &Logger::GetLogs()
	{
		return m_Logs;
	}

	std::string Logger::GetTime()
	{
		std::time_t time = std::time(nullptr);
		struct tm	tstruct;
		char		buff[80];
		tstruct = *std::localtime(&time);
		strftime(buff, sizeof(buff), "%d-%m-%Y %X", &tstruct);

		std::string text = buff;
		return text;
	}

	Logger *GetCoreLogger()
	{
		return logger;
	}
}	 // namespace Nexus