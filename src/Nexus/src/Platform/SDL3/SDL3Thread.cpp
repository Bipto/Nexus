#include "SDL3Thread.hpp"

namespace Nexus::Threading
{
	static SDL_ThreadPriority GetSDLThreadPriority(ThreadPriority priority)
	{
		switch (priority)
		{
			case ThreadPriority::Low: return SDL_THREAD_PRIORITY_LOW;
			case ThreadPriority::Normal: return SDL_THREAD_PRIORITY_NORMAL;
			case ThreadPriority::High: return SDL_THREAD_PRIORITY_HIGH;
			case ThreadPriority::TimeCritical: return SDL_THREAD_PRIORITY_TIME_CRITICAL;
			case ThreadPriority::Default:
			default: throw std::runtime_error("Failed to find a valid thread priority");
		}
	}

	static ThreadState GetNXThreadState(SDL_ThreadState state)
	{
		switch (state)
		{
			case SDL_THREAD_UNKNOWN: return ThreadState::Uknown;
			case SDL_THREAD_ALIVE: return ThreadState::Alive;
			case SDL_THREAD_DETACHED: return ThreadState::Detached;
			case SDL_THREAD_COMPLETE: return ThreadState::Complete;
			default: throw std::runtime_error("Failed to find a valid thread state");
		}
	}

	SDL3Thread::SDL3Thread(const ThreadDescription &description, std::function<void()> function) : m_Description(description)
	{
		m_Function = std::move(function);

		SDL_PropertiesID properties = SDL_CreateProperties();
		SDL_SetStringProperty(properties, SDL_PROP_THREAD_CREATE_NAME_STRING, description.Name.c_str());
		SDL_SetNumberProperty(properties, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, description.StackSize);
		SDL_SetPointerProperty(properties, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, (void *)ThreadEntry);
		SDL_SetPointerProperty(properties, SDL_PROP_THREAD_CREATE_USERDATA_POINTER, this);

		auto threadDeleter = [](SDL_Thread *thread) { SDL_WaitThread(thread, nullptr); };
		m_Thread		   = {SDL_CreateThreadWithProperties(properties), threadDeleter};
		SDL_DestroyProperties(properties);
	}

	SDL3Thread::~SDL3Thread()
	{
		if (m_Thread)
		{
			SDL_WaitThread(m_Thread.get(), nullptr);
		}
	}

	void SDL3Thread::Join()
	{
		if (m_Thread)
		{
			SDL_WaitThread(m_Thread.get(), nullptr);
			m_Thread = nullptr;
		}
	}

	const ThreadDescription &SDL3Thread::GetDescription() const
	{
		return m_Description;
	}

	ThreadState SDL3Thread::GetThreadState() const
	{
		SDL_ThreadState threadState = SDL_GetThreadState(m_Thread.get());
		return GetNXThreadState(threadState);
	}

	void SDL3Thread::Wait() const
	{
		SDL_WaitThread(m_Thread.get(), nullptr);
	}

	void SDL3Thread::Detach() const
	{
		SDL_DetachThread(m_Thread.get());
	}

	// this is the entry point for all SDL3 threads
	int SDL3Thread::ThreadEntry(void *data)
	{
		auto *self = static_cast<SDL3Thread *>(data);

		ThreadPriority priority = self->m_Description.Priority;
		if (priority != ThreadPriority::Default)
		{
			SDL_ThreadPriority sdlPriority = GetSDLThreadPriority(priority);
			SDL_SetCurrentThreadPriority(sdlPriority);
		}

		if (self && self->m_Function)
		{
			self->m_Function();
		}
		return 0;
	}

}	 // namespace Nexus::Threading