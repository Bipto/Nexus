#pragma once

#include <string>
#include <vector>

#include "Platform/Platform-Core.hpp"
#include "Platform/Timings/Profiler.hpp"
#include "Platform/Timings/Timer.hpp"
#include "Platform/Timings/Timespan.hpp"

namespace Nexus::Timings
{
	struct ProfileResult
	{
		std::string Name = {};
		TimeSpan	Time = {};
	};

	class NX_PLATFORM_API Profiler
	{
	  public:
		void							  AddResult(const std::string &name, TimeSpan timespan);
		const std::vector<ProfileResult> &GetResults() const;
		void							  Reset();

		static Profiler &Get();

	  private:
		std::vector<ProfileResult> m_Results = {};
		Profiler()							 = default;
	};
}	 // namespace Nexus::Timings

// #define NX_PROFILING_ENABLE