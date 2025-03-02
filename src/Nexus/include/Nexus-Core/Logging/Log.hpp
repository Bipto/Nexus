#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief An enum representing the severity of a log message
	enum class Severity
	{
		/// @brief The message provides useful debugging data
		Info,

		/// @brief The message contains a potential problem that may reduce
		/// performance or cause crashes
		Warning,

		/// @brief The message contains an fatal issue with the application that will
		/// result in a crash
		Error
	};

	/// @brief A struct representing all of the information about a log message
	struct Log
	{
	  public:
		/// @brief The text of the log message
		std::string Message;

		/// @brief An enum value represenging how severe the error was
		Severity MessageSeverity;

		/// @brief The time that the log was produced
		std::chrono::system_clock::time_point Time;

	  public:
		/// @brief An empty log cannot be created
		Log() = delete;

		/// @brief A constructor that takes in a message and a severity
		/// @param message The text message of the log
		/// @param severity The severity of the log
		Log(const std::string &message, Severity severity);
	};

	/// @brief A class that allows logs to be recorded and retrieve later
	class Logger
	{
	  public:
		/// @brief A method that adds a new info log to the logger
		/// @param message The text of the error message
		void LogInfo(const std::string &message, const source_location location = source_location::current());

		/// @brief A method that adds a new warning log to the logger
		/// @param message The text of the error message
		void LogWarning(const std::string &message, const source_location location = source_location::current());

		/// @brief A method that adds a new error log to the logger
		/// @param message The text of the error message
		void LogError(const std::string &message, const source_location location = source_location::current());

		/// @brief A method that returns the logs stored within the logger
		/// @return A const reference to the vector of logs
		const std::vector<Nexus::Log> &GetLogs();

	  private:
		/// @brief A method that returns the current time as a string
		/// @return A const reference to a string containing the current time
		static std::string GetTime();

	  private:
		/// @brief A vector containing the logger's logs
		std::vector<Nexus::Log> m_Logs;
	};

	/// @brief A method that returns a raw pointer to the engine's core logger
	/// @return A raw pointer to the core logger of the engine
	Logger *GetCoreLogger();
}	 // namespace Nexus

#define NX_LOG(message)				   Nexus::GetCoreLogger()->LogInfo(message)
#define NX_WARNING(message)			   Nexus::GetCoreLogger()->LogWarning(message)
#define NX_ERROR(message)			   Nexus::GetCoreLogger()->LogError(message)
#define NX_ASSERT(expression, message) assert(expression &&message)