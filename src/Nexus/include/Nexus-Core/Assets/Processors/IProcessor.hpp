#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{

	class Project;
}

namespace Nexus::Processors
{
	class NX_API IProcessor
	{
	  public:
		IProcessor(const std::string &name) : m_Name(name)
		{
		}

		virtual ~IProcessor()																				  = default;
		virtual GUID Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project) = 0;
		virtual std::any Import(const std::string &filepath)													  = 0;

		const std::string &GetName() const
		{
			return m_Name;
		}

	  private:
		std::string m_Name = {};
	};

	struct ProcessorInfo
	{
		std::function<IProcessor *()> CreationFunction = {};
		std::vector<std::string>	  FileExtensions   = {};
	};

	class ProcessorRegistry
	{
	  public:
		static std::map<std::string, ProcessorInfo> &GetRegistry()
		{
			static std::map<std::string, ProcessorInfo> registry;
			return registry;
		}

		static std::optional<ProcessorInfo> GetProcessorInfo(const std::string &name)
		{
			auto &registry = GetRegistry();
			if (registry.contains(name))
			{
				return registry[name];
			}

			return {};
		}

		template<typename T>
		static void AddProcessor(const std::string &name, const std::vector<std::string> &extensions)
		{
			ProcessorInfo info	  = {};
			info.CreationFunction = []() { return new T(); };
			info.FileExtensions	  = extensions;

			auto &processors = GetRegistry();
			processors[name] = info;
		}
	};

	extern "C++" inline NX_API std::map<std::string, ProcessorInfo> &GetAssetProcessorRegistry()
	{
		return ProcessorRegistry::GetRegistry();
	}

}	 // namespace Nexus::Processors

#define NX_REGISTER_PROCESSOR(ProcessorType, ProcessorName, Extensions)                                                                              \
	struct ProcessorType##Register                                                                                                                   \
	{                                                                                                                                                \
		ProcessorType##Register()                                                                                                                    \
		{                                                                                                                                            \
			ProcessorRegistry::AddProcessor<ProcessorType>(ProcessorName, Extensions);                                                               \
		}                                                                                                                                            \
	};                                                                                                                                               \
	static ProcessorType##Register instance##ProcessorType##Register;                                                                                \
	\