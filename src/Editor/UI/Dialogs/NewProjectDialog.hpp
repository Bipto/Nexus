#pragma once

#include "UI/Panel.hpp"

namespace Editor
{
class NewProjectDialog : public Panel
{
  public:
    virtual void OnLoad() override;
    virtual void OnRender() override;

  private:
    std::string m_Name{"Test Project"};
    std::string m_Path{};

    std::string m_ProjectFilePath;
};
} // namespace Editor