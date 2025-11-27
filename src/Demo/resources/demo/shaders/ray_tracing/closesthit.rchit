#version 460
#extension GL_EXT_ray_tracing : require

layout (location = 0) rayPayloadInEXT vec3 payload;

hitAttributeEXT vec2 attribs;

void main()
{
	//color by barycentric coordinates
	payload = vec3(attribs, 1.0 - attribs.x - attribs.y);
}