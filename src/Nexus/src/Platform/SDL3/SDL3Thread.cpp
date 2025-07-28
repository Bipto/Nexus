#include "SDL3Thread.hpp"

namespace Nexus
{
	Nexus::SDL3Thread::SDL3Thread(const ThreadDescription &description, std::function<void()> function) : m_Description(description)
	{
		m_Function = std::move(function);

		SDL_PropertiesID properties = SDL_CreateProperties();
		SDL_SetStringProperty(properties, SDL_PROP_THREAD_CREATE_NAME_STRING, description.Name.c_str());
		SDL_SetNumberProperty(properties, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, description.StackSize);
		SDL_SetPointerProperty(properties, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, ThreadEntry);
		SDL_SetPointerProperty(properties, SDL_PROP_THREAD_CREATE_USERDATA_POINTER, this);

		m_Thread = SDL_CreateThreadWithProperties(properties);
		SDL_DestroyProperties(properties);
	}

	SDL3Thread::~SDL3Thread()
	{
		if (m_Thread)
		{
			SDL_WaitThread(m_Thread, nullptr);
		}
	}

	void SDL3Thread::Join()
	{
		if (m_Thread)
		{
			SDL_WaitThread(m_Thread, nullptr);
			m_Thread = nullptr;
		}
	}

	const ThreadDescription &SDL3Thread::GetDescription() const
	{
		return m_Description;
	}

	int SDL3Thread::ThreadEntry(void *data)
	{
		auto *self = static_cast<SDL3Thread *>(data);
		if (self && self->m_Function)
		{
			self->m_Function();
		}
		return 0;
	}

}	 // namespace Nexus