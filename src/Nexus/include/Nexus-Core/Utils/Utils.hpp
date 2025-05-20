#pragma once

#include "Nexus-Core/Graphics/PixelFormat.hpp"
#include "Nexus-Core/Graphics/Polygon.hpp"
#include "Nexus-Core/Graphics/SamplerState.hpp"
#include "Nexus-Core/Graphics/Triangle.hpp"
#include "Nexus-Core/Graphics/WindingOrder.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class DeviceBuffer;
	class GraphicsDevice;
}	 // namespace Nexus::Graphics

namespace Nexus::Utils
{
	NX_API glm::vec4 ColorFromRGBA(float r, float g, float b, float a);
	NX_API glm::vec4 ColorFromBorderColor(Nexus::Graphics::BorderColor color);
	NX_API glm::vec4 GenerateRandomColour();

	template<typename T>
	inline T ReMapRange(T oldMin, T oldMax, T newMin, T newMax, T value)
	{
		return newMin + (newMax - newMin) * ((value - oldMin) / (oldMax - oldMin));
	}

	template<typename T>
	inline T Lerp(T x, T y, T t)
	{
		return x * (1 - t) + y * t;
	}

	inline float Dot(const glm::vec2 &a, const glm::vec2 &b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float Cross(const glm::vec2 &a, const glm::vec2 &b)
	{
		return a.x * b.y - b.x * a.y;
	}

	inline bool IsInsideEdge(const glm::vec2 &point, const glm::vec2 &a, const glm::vec2 &b)
	{
		return (Cross(a - b, point) + Cross(b, a)) < 0.0f;
	}

	inline glm::vec2 Intersection(const glm::vec2 &a1, const glm::vec2 &a2, const glm::vec2 &b1, const glm::vec2 &b2)
	{
		return ((b1 - b2) * Cross(a1, a2) - (a1 - a2) * Cross(b1, b2)) * (1.0f / Cross(a1 - a2, b1 - b2));
	}

	NX_API void Clip(std::vector<glm::vec2> &points, float x1, float y1, float x2, float y2);

	NX_API std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2> &subjectPolygon, const std::vector<glm::vec2> &clipPolygon);
	NX_API Nexus::Graphics::Polygon SutherlandHodgman(const Nexus::Graphics::Polygon &subject, const Nexus::Graphics::Polygon &clip);

	NX_API float FindPolygonArea(std::span<glm::vec2> polygon);

	NX_API bool Triangulate(const std::vector<glm::vec2> &polygon, std::vector<uint32_t> &triangles);

	NX_API std::vector<Nexus::Graphics::Triangle2D> GenerateGeometry(const std::vector<glm::vec2> &polygon, const std::vector<uint32_t> &indices);

	NX_API Nexus::Graphics::Polygon GeneratePolygon(const std::vector<glm::vec2> &polygon);

	NX_API Graphics::WindingOrder GetWindingOrder(glm::vec2 a, glm::vec2 b, glm::vec2 c);

	NX_API std::vector<glm::vec2> ReverseWindingOrder(const std::vector<glm::vec2> &vertices);

	template<typename T>
	T GetItem(std::span<T> collection, int index)
	{
		int32_t size = static_cast<int32_t>(collection.size());

		if (index >= size)
		{
			return collection[index % collection.size()];
		}
		else if (index < 0)
		{
			uint32_t i = collection.size() - (index * -1);
			return collection[i];
		}
		else
		{
			return collection[index];
		}
	}

	template<typename T>
	inline bool Contains(std::vector<T> items, T toFind)
	{
		return std::find(items.begin(), items.end(), toFind) != items.end();
	}

	inline uint32_t CalculateSubresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t totalMipLevels)
	{
		return mipLevel + arrayLayer * totalMipLevels;
	}

	NX_API uint64_t GetCurrentTimeAsInt();

	// djb2 algorithm
	inline size_t Hash(const std::string &text)
	{
		size_t hash = 5381;
		for (char c : text) { hash = ((hash << 5) + hash) + c; }

		return hash;
	}

	inline Point2D<uint32_t> GetMipSize(uint32_t baseWidth, uint32_t baseHeight, uint32_t level)
	{
		uint32_t width	= baseWidth;
		uint32_t height = baseHeight;

		for (uint32_t i = 0; i < level; i++)
		{
			width /= 2;
			height /= 2;
		}

		return {width, height};
	}

	inline size_t Hash(const std::vector<unsigned char> &buffer)
	{
		size_t hash = 5381;
		for (unsigned char c : buffer) { hash = ((hash << 5) + hash + c); }
		return hash;
	}

	Ref<Graphics::DeviceBuffer> CreateUploadBuffer(const void *data, size_t sizeInBytes, size_t strideInBytes, Graphics::GraphicsDevice *device);
	Ref<Graphics::DeviceBuffer>				CreateFilledVertexBuffer(const void				  *data,
																	 size_t					   sizeInBytes,
																	 size_t					   strideInBytes,
																	 Graphics::GraphicsDevice *device);
	Ref<Graphics::DeviceBuffer> CreateFilledIndexBuffer(const void *data, size_t sizeInBytes, size_t strideInBytes, Graphics::GraphicsDevice *device);
	Ref<Graphics::DeviceBuffer> CreateFilledUniformBuffer(const void			   *data,
														  size_t					sizeInBytes,
														  size_t					strideInBytes,
														  Graphics::GraphicsDevice *device);

	template<typename T>
	T AlignTo(T value, T alignment)
	{
		return std::ceil(value / alignment) * alignment;
	}

	void ConvertNanosecondsToTm(uint64_t nanoseconds, std::tm &outTime);

	NX_API void FlipPixelsHorizontally(void *pixels, uint32_t width, uint32_t height, Graphics::PixelFormat format);
	NX_API void FlipPixelsVertically(void *pixels, uint32_t width, uint32_t height, Graphics::PixelFormat format);

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)

}	 // namespace Nexus::Utils

#define BIT(x) (1 << x)