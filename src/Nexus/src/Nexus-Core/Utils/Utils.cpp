#include "Nexus-Core/Utils/Utils.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Utils
{
	glm::vec4 ColorFromRGBA(float r, float g, float b, float a)
	{
		return glm::vec4(1.0f / 255.0f * r, 1.0f / 255.0f * g, 1.0f / 255.0f * b, 1.0f / 255.0f * a);
	}

	glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color)
	{
		switch (color)
		{
			case Nexus::Graphics::BorderColor::TransparentBlack: return {0.0f, 0.0f, 0.0f, 0.0f};
			case Nexus::Graphics::BorderColor::OpaqueBlack: return {0.0f, 0.0f, 0.0f, 1.0f};
			case Nexus::Graphics::BorderColor::OpaqueWhite: return {1.0f, 1.0f, 1.0f, 1.0f};
			default: throw std::runtime_error("Failed to find a valid border color");
		}
	}

	NX_API glm::vec4 GenerateRandomColour()
	{
		static std::random_device		 rd;
		static std::mt19937				 gen(rd());
		std::uniform_real_distribution<> dis(0.0f, 1.0f);
		return glm::vec4(dis(gen), dis(gen), dis(gen), 1.0f);
	}
	float XIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
	{
		float num = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
		float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		return num / den;
	}

	float YIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
	{
		float num = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
		float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		return num / den;
	}

	void Clip(std::vector<glm::vec2> &points, float x1, float y1, float x2, float y2)
	{
		std::vector<glm::vec2> newPoints;

		for (size_t i = 0; i < points.size(); i++)
		{
			size_t	  k	 = (i + 1) % points.size();
			glm::vec2 pi = points[i];
			glm::vec2 pk = points[k];

			float i_pos = (x2 - x1) * (pi.y - y1) - (y2 - y1) * (pi.x - x1);
			float k_pos = (x2 - x1) * (pk.y - y1) - (y2 - y1) * (pk.x - x1);

			if (i_pos < 0 && k_pos < 0)
			{
				newPoints.push_back(pk);
			}
			else if (i_pos >= 0 && k_pos < 0)
			{
				float newX = XIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);
				float newY = YIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);

				newPoints.push_back(glm::vec2(newX, newY));
				newPoints.push_back(pk);
			}
			else if (i_pos < 0 && k_pos >= 0)
			{
				float newX = XIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);
				float newY = YIntersect(x1, y1, x2, y2, pi.x, pi.y, pk.x, pk.y);

				newPoints.push_back(glm::vec2(newX, newY));
			}
		}

		points = newPoints;
	}

	std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2> &subjectPolygon, const std::vector<glm::vec2> &clipPolygon)
	{
		assert(subjectPolygon.size() >= 3 && "The subject polygon must have at least 3 points");
		assert(clipPolygon.size() >= 3 && "The clipping polygon must have at least 3 points");

		std::vector<glm::vec2> poly(subjectPolygon);
		std::vector<glm::vec2> clip(clipPolygon);

		Graphics::Triangle2D subjectTri;
		subjectTri.A = poly[0];
		subjectTri.B = poly[1];
		subjectTri.C = poly[2];

		Graphics::Triangle2D clipTri;
		clipTri.A = clip[0];
		clipTri.B = clip[1];
		clipTri.C = clip[2];

		Graphics::WindingOrder subjectWindingOrder = subjectTri.GetWindingOrder();
		Graphics::WindingOrder clipWindingOrder	   = clipTri.GetWindingOrder();

		if (subjectWindingOrder == Graphics::WindingOrder::CounterClockwise)
		{
			poly = ReverseWindingOrder(poly);
		}

		if (clipWindingOrder == Graphics::WindingOrder::CounterClockwise)
		{
			clip = ReverseWindingOrder(clipPolygon);
		}

		for (size_t i = 0; i < clip.size(); i++)
		{
			size_t k = (i + 1) % clip.size();

			Clip(poly, clip[i].x, clip[i].y, clip[k].x, clip[k].y);
		}

		return poly;
	}

	NX_API Nexus::Graphics::Polygon SutherlandHodgman(const Nexus::Graphics::Polygon &subject, const Nexus::Graphics::Polygon &clip)
	{
		std::vector<glm::vec2> subjectPoints = subject.GetOutline();
		std::vector<glm::vec2> clipPoints	 = clip.GetOutline();

		std::vector<glm::vec2> newPoints = SutherlandHodgman(subjectPoints, clipPoints);

		return Nexus::Utils::GeneratePolygon(newPoints);
	}

	float FindPolygonArea(std::span<glm::vec2> polygon)
	{
		float totalArea = 0.0f;

		for (size_t i = 0; i < polygon.size(); i++)
		{
			glm::vec2 a = polygon[i];
			glm::vec2 b = polygon[(i + 1) % polygon.size()];

			float dy = (a.y + b.y) / 2.0f;
			float dx = (b.x - a.x);

			float area = dy * dx;
			totalArea += area;
		}

		return glm::abs(totalArea);
	}

	bool IsPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c)
	{
		glm::vec2 ab = b - a;
		glm::vec2 bc = c - b;
		glm::vec2 ca = a - c;

		glm::vec2 ap = p - a;
		glm::vec2 bp = p - b;
		glm::vec2 cp = p - c;

		float cross1 = Cross(ab, ap);
		float cross2 = Cross(bc, bp);
		float cross3 = Cross(ca, cp);

		if (cross1 > 0.0f || cross2 > 0.0f || cross3 > 0.0f)
		{
			return false;
		}

		return true;
	}

	bool Triangulate(const std::vector<glm::vec2> &polygon, std::vector<uint32_t> &triangles)
	{
		std::vector<uint32_t> indexList;
		for (size_t i = 0; i < polygon.size(); i++) { indexList.push_back(i); }

		while (indexList.size() > 3)
		{
			for (size_t i = 0; i < indexList.size(); i++)
			{
				uint32_t a = GetItem<uint32_t>(indexList, i);
				uint32_t b = GetItem<uint32_t>(indexList, i - 1);
				uint32_t c = GetItem<uint32_t>(indexList, i + 1);

				Graphics::Triangle2D tri;
				tri.A = polygon[a];
				tri.B = polygon[b];
				tri.C = polygon[c];

				bool isEar = true;

				for (size_t j = 0; j < polygon.size(); j++)
				{
					if (j == a || j == b || j == c)
					{
						continue;
					}

					glm::vec2 p = polygon[j];

					if (tri.Contains(p))
					{
						isEar = false;
						break;
					}
				}

				if (isEar)
				{
					triangles.push_back(a);
					triangles.push_back(b);
					triangles.push_back(c);

					indexList.erase(indexList.begin() + i);
					break;
				}
			}
		}

		if (triangles.empty())
		{
			return false;
		}

		triangles.push_back(indexList[0]);
		triangles.push_back(indexList[1]);
		triangles.push_back(indexList[2]);

		return true;
	}

	std::vector<Nexus::Graphics::Triangle2D> GenerateGeometry(const std::vector<glm::vec2> &polygon, const std::vector<uint32_t> &indices)
	{
		std::vector<Nexus::Graphics::Triangle2D> triangles;

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Nexus::Graphics::Triangle2D tri;
			tri.A = polygon[indices[i]];
			tri.B = polygon[indices[(i + 1) % indices.size()]];
			tri.C = polygon[indices[(i + 2) % indices.size()]];

			triangles.push_back(tri);
		}

		return triangles;
	}

	Nexus::Graphics::Polygon GeneratePolygon(const std::vector<glm::vec2> &polygon)
	{
		std::vector<uint32_t> indices;
		if (Triangulate(polygon, indices))
		{
			std::vector<Nexus::Graphics::Triangle2D> tris = GenerateGeometry(polygon, indices);
			Nexus::Graphics::Polygon				 poly(tris);
			return poly;
		}

		// return an empty polygon if we are not able to create a valid one
		return Nexus::Graphics::Polygon();
	}

	Graphics::WindingOrder GetWindingOrder(glm::vec2 a, glm::vec2 b, glm::vec2 c)
	{
		return ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y)) < 0 ? Graphics::WindingOrder::Clockwise
																		   : Graphics::WindingOrder::CounterClockwise;
	}

	std::vector<glm::vec2> ReverseWindingOrder(const std::vector<glm::vec2> &vertices)
	{
		std::vector<glm::vec2> newVertices {vertices.begin(), vertices.end()};
		std::reverse(newVertices.begin(), newVertices.end());
		return newVertices;
	}

	uint64_t GetCurrentTimeAsInt()
	{
		auto now	  = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	Ref<Graphics::DeviceBuffer> CreateUploadBuffer(const void *data, size_t sizeInBytes, size_t strideInBytes, Graphics::GraphicsDevice *device)
	{
		Nexus::Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Upload;
		bufferDesc.StrideInBytes							= strideInBytes;
		bufferDesc.SizeInBytes								= sizeInBytes;

		Ref<Graphics::DeviceBuffer> buffer = device->CreateDeviceBuffer(bufferDesc);
		buffer->SetData(data, 0, sizeInBytes);
		return buffer;
	}

	Ref<Graphics::DeviceBuffer> CreateFilledVertexBuffer(const void *data, size_t sizeInBytes, size_t strideInBytes, Graphics::GraphicsDevice *device)
	{
		Ref<Graphics::DeviceBuffer>				uploadBuffer = CreateUploadBuffer(data, sizeInBytes, strideInBytes, device);
		Ref<Graphics::CommandList>				commandList	 = device->CreateCommandList();

		Nexus::Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Vertex;
		bufferDesc.StrideInBytes							= strideInBytes;
		bufferDesc.SizeInBytes								= sizeInBytes;
		Ref<Graphics::DeviceBuffer> vertexBuffer			= Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(bufferDesc));

		Nexus::Graphics::BufferCopyDescription bufferCopy = {};
		bufferCopy.Source								  = uploadBuffer;
		bufferCopy.Destination							  = vertexBuffer;
		bufferCopy.ReadOffset							  = 0;
		bufferCopy.WriteOffset							  = 0;
		bufferCopy.Size									  = sizeInBytes;

		commandList->Begin();
		commandList->CopyBufferToBuffer(bufferCopy);
		commandList->End();
		device->SubmitCommandLists(&commandList, 1, nullptr);
		device->WaitForIdle();

		return vertexBuffer;
	}

	Ref<Graphics::DeviceBuffer> CreateFilledIndexBuffer(const void *data, size_t sizeInBytes, size_t strideInBytes, Graphics::GraphicsDevice *device)
	{
		Ref<Graphics::DeviceBuffer>				uploadBuffer = CreateUploadBuffer(data, sizeInBytes, strideInBytes, device);
		Ref<Graphics::CommandList>				commandList	 = device->CreateCommandList();

		Nexus::Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Index;
		bufferDesc.StrideInBytes							= strideInBytes;
		bufferDesc.SizeInBytes								= sizeInBytes;
		Ref<Graphics::DeviceBuffer> indexBuffer				= Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(bufferDesc));

		Nexus::Graphics::BufferCopyDescription bufferCopy = {};
		bufferCopy.Source								  = uploadBuffer;
		bufferCopy.Destination							  = indexBuffer;
		bufferCopy.ReadOffset							  = 0;
		bufferCopy.WriteOffset							  = 0;
		bufferCopy.Size									  = sizeInBytes;

		commandList->Begin();
		commandList->CopyBufferToBuffer(bufferCopy);
		commandList->End();
		device->SubmitCommandLists(&commandList, 1, nullptr);
		device->WaitForIdle();

		return indexBuffer;
	}

	Ref<Graphics::DeviceBuffer> CreateFilledUniformBuffer(const void			   *data,
														  size_t					sizeInBytes,
														  size_t					strideInBytes,
														  Graphics::GraphicsDevice *device)
	{
		Ref<Graphics::DeviceBuffer>				uploadBuffer = CreateUploadBuffer(data, sizeInBytes, strideInBytes, device);
		Ref<Graphics::CommandList>				commandList	 = device->CreateCommandList();

		Nexus::Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Uniform;
		bufferDesc.StrideInBytes							= strideInBytes;
		bufferDesc.SizeInBytes								= sizeInBytes;
		Ref<Graphics::DeviceBuffer> uniformBuffer			= Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(bufferDesc));

		Nexus::Graphics::BufferCopyDescription bufferCopy = {};
		bufferCopy.Source								  = uploadBuffer;
		bufferCopy.Destination							  = uniformBuffer;
		bufferCopy.ReadOffset							  = 0;
		bufferCopy.WriteOffset							  = 0;
		bufferCopy.Size									  = sizeInBytes;

		commandList->Begin();
		commandList->CopyBufferToBuffer(bufferCopy);
		commandList->End();
		device->SubmitCommandLists(&commandList, 1, nullptr);
		device->WaitForIdle();

		return uniformBuffer;
	}

	void ConvertNanosecondsToTm(uint64_t nanoseconds, std::tm &outTime)
	{
		std::time_t seconds = nanoseconds / 1'000'000'000;
		localtime_s(&outTime, &seconds);
	}

	void FlipPixelsHorizontally(void *pixels, uint32_t width, uint32_t height, Graphics::PixelFormat format)
	{
		unsigned char *bufferPointer = (unsigned char *)pixels;

		uint32_t bytesPerPixel	  = Graphics::GetPixelFormatSizeInBytes(format);
		uint32_t channelsPerPixel = Graphics::GetPixelFormatNumberOfChannels(format);
		uint32_t bytesPerChannel  = bytesPerPixel / channelsPerPixel;

		for (uint32_t j = 0; j < height; ++j)
		{
			for (uint32_t i = 0; i < width / 2; ++i)
			{
				for (uint32_t k = 0; k < channelsPerPixel; ++k)
				{
					for (uint32_t b = 0; b < bytesPerChannel; ++b)
					{
						/* std::swap(pixels[j * width * bytesPerPixel + i * bytesPerPixel + k * bytesPerChannel + b],
								  pixels[j * width * bytesPerPixel + (width - 1 - i) * bytesPerPixel + k * bytesPerChannel + b]); */

						std::swap(*(bufferPointer + (j * width * bytesPerPixel + i * bytesPerPixel + k * bytesPerChannel + b)),
								  *(bufferPointer + (j * width * bytesPerPixel + (width - 1 - i) * bytesPerPixel + k * bytesPerChannel + b)));
					}
				}
			}
		}
	}

	void FlipPixelsVertically(void *pixels, uint32_t width, uint32_t height, Graphics::PixelFormat format)
	{
		unsigned char *bufferPointer = (unsigned char *)pixels;

		uint32_t bytesPerPixel	  = Graphics::GetPixelFormatSizeInBytes(format);
		uint32_t channelsPerPixel = Graphics::GetPixelFormatNumberOfChannels(format);
		uint32_t bytesPerChannel  = bytesPerPixel / channelsPerPixel;

		for (uint32_t j = 0; j < height / 2; ++j)
		{
			for (uint32_t i = 0; i < width; ++i)
			{
				for (uint32_t k = 0; k < channelsPerPixel; ++k)
				{
					for (uint32_t b = 0; b < bytesPerChannel; ++b)
					{
						std::swap(*(bufferPointer + (j * width * bytesPerPixel + i * bytesPerPixel + k * bytesPerChannel + b)),
								  *(bufferPointer + ((height - 1 - j) * width * bytesPerPixel + i * bytesPerPixel + k * bytesPerChannel + b)));
					}
				}
			}
		}
	}

}	 // namespace Nexus::Utils