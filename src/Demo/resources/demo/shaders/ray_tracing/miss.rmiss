#version 460
#extension GL_EXT_ray_tracing : require

layout (location = 0) rayPayloadInEXT vec3 payload;

void main()
{
	//simple sky colour
	payload = vec3(0.2, 0.4, 0.7);
}