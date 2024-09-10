#pragma once

namespace Nexus
{
	/// @brief A class that represents an event dispatched by the engine
	/// @tparam T A template that can represent any custom data used by an event
	template<typename T>
	class Event
	{
	  public:
		/// @brief A default constructor is not available for events
		Event() = delete;

		/// @brief The constructor used by events, allowing a custom payload to be
		/// provided
		/// @param payload The payload of the event
		Event(const T &payload)
		{
			m_Payload = payload;
		}

		/// @brief A method that returns the payload of the event
		/// @return A const reference to the payload
		const T &GetPayload()
		{
			return m_Payload;
		}

	  private:
		/// @brief A custom payload
		T m_Payload;
	};
}	 // namespace Nexus