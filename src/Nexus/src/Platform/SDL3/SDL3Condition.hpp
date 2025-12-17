#pragma once

#include "Nexus-Core/Threading/Condition.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3Condition final : public ConditionBase
	{
	  public:
		SDL3Condition();
		virtual ~SDL3Condition() = default;
		virtual void Signal() const final;
		virtual void BroadCast() const final;
		virtual void Wait(const Mutex &mutex) const final;
		virtual bool Wait(const Mutex &mutex, TimeSpan timeout) const final;

	  private:
		std::unique_ptr<SDL_Condition, std::function<void(SDL_Condition *)>> m_Condition = {};
	};
}	 // namespace Nexus::Threading