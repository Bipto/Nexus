#include "Nexus.hpp"

void *operator new(size_t size)
{
	s_AllocationTracker.TotalAllocated += size;
	return malloc(size);
}

void operator delete(void *memory, size_t size)
{
	s_AllocationTracker.TotalFreed += size;
	free(memory);
}
