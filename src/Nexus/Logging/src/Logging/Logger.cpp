#include "Logging/Logger.hpp"

#include <algorithm>
#include <optional>
#include <ranges>

namespace Nexus::Logging
{
    void Logger::RemoveSink(ILogSink *sink)
    {
        std::erase_if(m_Sinks, [sink](const std::unique_ptr<ILogSink> &item) { return item.get() == sink; });
    }

    size_t Logger::GetSinkCount() const
    {
        return m_Sinks.size();
    }
} // namespace Nexus::Logging