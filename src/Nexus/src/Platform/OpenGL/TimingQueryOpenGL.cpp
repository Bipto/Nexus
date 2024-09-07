#if defined(NX_PLATFORM_OPENGL)

#include "TimingQueryOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics {
TimingQueryOpenGL::TimingQueryOpenGL() {}

float TimingQueryOpenGL::GetElapsedMilliseconds() { return m_ElapsedTime; }

void TimingQueryOpenGL::Resolve() {
  m_ElapsedTime = (m_End - m_Start) / 1000000.0f;
}
} // namespace Nexus::Graphics

#endif