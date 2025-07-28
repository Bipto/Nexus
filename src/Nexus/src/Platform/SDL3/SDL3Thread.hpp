#include "Nexus-Core/nxpch.hpp"
#include "SDL3Include.hpp"

#include "Nexus-Core/Threading/Thread.hpp"

namespace Nexus::Threading
{
	class SDL3Thread final : public ThreadBase
	{
	  public:
		explicit SDL3Thread(const ThreadDescription &description, std::function<void()> function);
		virtual ~SDL3Thread();
		void Join();

		const ThreadDescription &GetDescription() const final;
		ThreadState				 GetThreadState() const final;
		void					 Wait() const final;
		void					 Detach() const final;

	  private:
		ThreadDescription	  m_Description;
		SDL_Thread			 *m_Thread;
		std::function<void()> m_Function;

		static int ThreadEntry(void *data);
	};
}	 // namespace Nexus
