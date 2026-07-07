#include "Logging/Logger.hpp"

namespace Nexus::Logging
{
	void Logger::RegisterSink(std::unique_ptr<ILogSink> sink)
	{ m_Sinks.push_back(std::move(sink)); }
}	 // namespace Nexus::Logging