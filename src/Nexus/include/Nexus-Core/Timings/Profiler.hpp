#pragma once

#include "Nexus-Core/Timings/Profiler.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Timings
{
	struct ProfileResult
	{
		std::string Name = {};
		TimeSpan	Time = {};
	};

	class Profiler
	{
	  public:
		void							  AddResult(const std::string &name, TimeSpan timespan);
		const std::vector<ProfileResult> &GetResults() const;
		void							  Reset();

		static Profiler &Get();

	   private:
		 std::vector<ProfileResult> m_Results = {};
		 Profiler()							  = default;
	};
}	 // namespace Nexus::Timings

#define NX_PROFILING_ENABLE 1

#if defined(NX_PROFILING_ENABLE)
	#define NX_PROFILE_FUNCTION()                                                                                                                    \
		Nexus::Timings::ProfilingTimer timer("Function");                                                                                            \
		timer.OnStop.Bind([&](Nexus::TimeSpan timespan) { Nexus::Timings::Profiler::Get().AddResult(timer.GenerateName(), timespan); });

	#define NX_PROFILE_SCOPE(name)                                                                                                                   \
		Nexus::Timings::ProfilingTimer timer(name);                                                                                                  \
		timer.OnStop.Bind([&](Nexus::TimeSpan timespan) { Nexus::Timings::Profiler::Get().AddResult(timer.GenerateName(), timespan); });
#else
	#define NX_PROFILE_FUNCTION
	#define NX_PROFILE_SCOPE(name)
#endif