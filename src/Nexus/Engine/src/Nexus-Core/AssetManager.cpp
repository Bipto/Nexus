#include "Nexus-Core/AssetManager.hpp"
#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Utils/GraphicsUtils.hpp"

namespace Nexus
{
    Graphics::TextureHandle AssetManager::GetTexture(const std::string &filepath)
    {
        return Utils::CreateTexture2D(m_CommandQueue, filepath.c_str(), false);
    }

    std::any AssetManager::LoadAsset(GUID id)
    {
        const Nexus::Assets::AssetRegistry &registry = m_Project->GetAssetRegistry();

        std::optional<Assets::AssetInfo> assetResult = registry.GetAssetInfo(id);
        if (!assetResult)
        {
            return {};
        }

        Assets::AssetInfo assetInfo = assetResult.value();

        std::optional<Nexus::Processors::ProcessorInfo> processorResult =
            m_Project->GetProcessorInfo(assetInfo.ProcessorName);
        if (processorResult)
        {
            return {};
        }

        return {};
    }

} // namespace Nexus
