#pragma once

template<typename Tag>
struct HandleT;
template<typename T, typename ID>
class SharedHandle;
template<typename T, typename ID>
class ResourcePool;

#define DEFINE_RESOURCE(ResourceName, ResourceType)                                                                                                  \
	struct ResourceName##Tag                                                                                                                         \
	{                                                                                                                                                \
	};                                                                                                                                               \
	using ResourceName##ID	   = Nexus::HandleT<ResourceName##Tag>;                                                                                  \
	using ResourceName##Handle = SharedHandle<ResourceType, ResourceName##ID>;                                                                       \
	using ResourceName##Pool   = ResourcePool<ResourceType, ResourceName##ID>;
