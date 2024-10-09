#include "Profiler.hpp"

namespace Nexus::Timings
{
	void Profiler::AddResult(const std::string &name, TimeSpan timespan)
	{
		m_Results.emplace_back(name, timespan);
	}

	const std::vector<ProfileResult> &Profiler::GetResults() const
	{
		return m_Results;
	}

	void Profiler::Reset()
	{
		m_Results.clear();
	}

	Profiler &Profiler::Get()
	{
		static Profiler profiler = {};
		return profiler;
	}

}	 // namespace Nexus::Timings