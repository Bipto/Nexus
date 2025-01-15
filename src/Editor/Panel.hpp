#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

class Panel
{
  public:
	Panel() = delete;
	~Panel()
	{
	}

	Panel(const std::string &name) : m_Name(name)
	{
	}

	void LoadProject(Nexus::Ref<Nexus::Project> project)
	{
		m_Project = project;
	}

	virtual void Render() = 0;

	void Open()
	{
		m_Open = true;
	}

	void Close()
	{
		m_Open = false;
	}

	bool IsOpen() const
	{
		return m_Open;
	}

	const std::string &GetName() const
	{
		return m_Name;
	}

	void OnEntitySelected(std::optional<Nexus::GUID> id)
	{
		m_SelectedEntity = id;
	}

  protected:
	std::string				   m_Name	 = {};
	Nexus::Ref<Nexus::Project> m_Project = nullptr;
	bool					   m_Open	 = false;
	std::optional<Nexus::GUID> m_SelectedEntity = {};
};