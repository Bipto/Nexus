#if defined(NX_PLATFORM_WEBGL)

	#include "ViewContextWebGL.hpp"

namespace Nexus::GL
{
	EM_JS(void, transfer_pixels_to_canvas, (const char *canvasId, const std::byte *pixels, int width, int height), {
		/* var canvas	  = document.getElementById(UTF8ToString(canvasId));
		var ctx		  = canvas.getContext('2d');
		var imageData = ctx.createImageData(width, height);
		var data	  = imageData.data;

		for (var i = 0; i < width * height * 4; ++i) { data[i] = Module.HEAPU8[pixels + i]; }

		// Flip the image vertically
		for (var y = 0; y < height / 2; ++y)
		{
			for (var x = 0; x < width; ++x)
			{
				var index1 = (y * width + x) * 4;
				var index2 = ((height - 1 - y) * width + x) * 4;
				for (var i = 0; i < 4; ++i)
				{
					var temp		 = data[index1 + i];
					data[index1 + i] = data[index2 + i];
					data[index2 + i] = temp;
				}
			}
		}

		ctx.putImageData(imageData, 0, 0); */

		var canvas	= document.getElementById(UTF8ToString(canvasId));
		var gl		= canvas.getContext('webgl2');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.texImage2D(gl.TEXTURE_2D,
					  0,
					  gl.RGBA,
					  width,
					  height,
					  0,
					  gl.RGBA,
					  gl.UNSIGNED_BYTE,
					  Module.HEAPU8.subarray(pixels, pixels + width * height * 4));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.clearColor(0.0, 0.0, 0.0, 1.0);
		gl.clear(gl.COLOR_BUFFER_BIT);	  // Create shaders
		var vsSource = ` attribute vec4 aVertexPosition;
		attribute vec2					aTextureCoord;
		varying highp vec2				vTextureCoord;
		void							main(void)
		{
			gl_Position	  = aVertexPosition;
			vTextureCoord = aTextureCoord;
		}
		`;
		var fsSource = ` varying highp vec2 vTextureCoord;
		uniform sampler2D					uSampler;
		void								main(void)
		{
			gl_FragColor = texture2D(uSampler, vTextureCoord);
		}
		`;
		function createShader(gl, source, type)
		{
			var shader = gl.createShader(type);
			gl.shaderSource(shader, source);
			gl.compileShader(shader);
			if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS))
			{
				console.log('An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader));
				return null;
			}
			return shader;
		}
		var vertexShader   = createShader(gl, vsSource, gl.VERTEX_SHADER);
		var fragmentShader = createShader(gl, fsSource, gl.FRAGMENT_SHADER);
		var shaderProgram  = gl.createProgram();
		gl.attachShader(shaderProgram, vertexShader);
		gl.attachShader(shaderProgram, fragmentShader);
		gl.linkProgram(shaderProgram);
		if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS))
		{
			console.log('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
			return;
		}
		gl.useProgram(shaderProgram);
		var vertexPosition = gl.getAttribLocation(shaderProgram, 'aVertexPosition');
		var textureCoord   = gl.getAttribLocation(shaderProgram, 'aTextureCoord');
		gl.enableVertexAttribArray(vertexPosition);
		gl.enableVertexAttribArray(textureCoord);	 // Set up vertices and texture coordinates
		var vertices = new Float32Array([-1.0, 1.0, 0.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 0.0]);
		var buffer	 = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(vertexPosition, 2, gl.FLOAT, false, 4 * 4, 0);
		gl.vertexAttribPointer(textureCoord, 2, gl.FLOAT, false, 4 * 4, 2 * 4);
		// Draw the texture
		gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
	});

	ViewContextWebGL::ViewContextWebGL(const std::string					 &canvasName,
									   Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice,
									   const ContextSpecification			 &spec)
		: m_Device(graphicsDevice),
		  m_Specification(spec),
		  m_CanvasName(canvasName)
	{
		int width, height;
		emscripten_get_canvas_element_size(m_CanvasName.c_str(), &width, &height);

		Graphics::FramebufferSpecification framebufferSpec		 = {};
		framebufferSpec.Width									 = (uint32_t)width;
		framebufferSpec.Height									 = (uint32_t)height;
		framebufferSpec.ColorAttachmentSpecification.Attachments = {Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
		framebufferSpec.DepthAttachmentSpecification			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;
		framebufferSpec.Samples									 = Graphics::SampleCount::SampleCount1;
		m_Framebuffer											 = graphicsDevice->CreateFramebuffer(framebufferSpec);
	}

	ViewContextWebGL::~ViewContextWebGL()
	{
	}

	bool Nexus::GL::ViewContextWebGL::MakeCurrent()
	{
		IOffscreenContext *context = m_Device->GetOffscreenContext();
		context->MakeCurrent();
		return true;
	}

	void ViewContextWebGL::BindAsRenderTarget()
	{
		Ref<Graphics::FramebufferOpenGL> framebufferGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
		if (framebufferGL)
		{
			framebufferGL->BindAsRenderTarget();
		}
	}

	void ViewContextWebGL::BindAsDrawTarget()
	{
		Ref<Graphics::FramebufferOpenGL> framebufferGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
		if (framebufferGL)
		{
			framebufferGL->BindAsDrawBuffer();
		}
	}

	void Nexus::GL::ViewContextWebGL::Swap()
	{
		uint32_t				 textureWidth  = m_Framebuffer->GetFramebufferSpecification().Width;
		uint32_t				 textureHeight = m_Framebuffer->GetFramebufferSpecification().Height;
		Ref<Graphics::Texture2D> texture	   = m_Framebuffer->GetColorTexture(0);
		std::vector<std::byte>	 pixels		   = texture->GetData(0, 0, 0, textureWidth, textureHeight);

		transfer_pixels_to_canvas(m_CanvasName.c_str(), pixels.data(), (int)textureWidth, (int)textureHeight);
	}

	void Nexus::GL::ViewContextWebGL::SetVSync(bool enabled)
	{
	}

	const ContextSpecification &Nexus::GL::ViewContextWebGL::GetSpecification() const
	{
		return m_Specification;
	}

}	 // namespace Nexus::GL

#endif