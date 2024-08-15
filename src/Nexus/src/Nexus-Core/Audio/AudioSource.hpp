#pragma once

#include "glm/glm.hpp"

namespace Nexus::Audio
{
/// @brief A pure virtual class representing a source that is able to play audio
class AudioSource
{
  public:
    /// @brief A virtual destructor to clean up resources
    virtual ~AudioSource()
    {
    }

    /// @brief A method that provides access to the location of the source
    /// @return
    virtual glm::vec3 &GetPosition() = 0;

    /// @brief A method that is used to set the location of the source
    /// @param position The current position of the source
    virtual void SetPosition(const glm::vec3 &position) = 0;
};
} // namespace Nexus::Audio