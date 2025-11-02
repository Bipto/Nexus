#if defined(NX_PLATFORM_OPENGL)

	#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
	ResourceSetOpenGL::ResourceSetOpenGL(Ref<Pipeline> pipeline) : IResourceSet(pipeline)
	{
	}

	void ResourceSetOpenGL::Flush()
	{
	}

}	 // namespace Nexus::Graphics

#endif