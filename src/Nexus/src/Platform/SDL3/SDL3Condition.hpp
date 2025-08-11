#pragma once

#include "Nexus-Core/Threading/Condition.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3Condition : public ConditionBase
	{
	  public:
		SDL3Condition();
		virtual ~SDL3Condition();
		virtual void Signal() final;
		virtual void BroadCast() final;
		virtual void Wait(Mutex &mutex) final;
		virtual bool Wait(Mutex &mutex, TimeSpan timeout) final;

	  private:
		SDL_Condition *m_Condition = nullptr;
	};
}	 // namespace Nexus::Threading