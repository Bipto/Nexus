#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0, set = 0) uniform image2D out_tex;

void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	if (pixelCoord.x >= imageSize(out_tex).x || pixelCoord.y >= imageSize(out_tex).y)
		return;

	ivec2 texSize		  = imageSize(out_tex);
	vec2  fTexSize		  = vec2(texSize);
	vec2  normalizedCoord = vec2(pixelCoord) / vec2(texSize);

	vec4 color = vec4(normalizedCoord, 0.0f, 1.0f);
	imageStore(out_tex, pixelCoord, color);
}