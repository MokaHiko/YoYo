#pragma once

#include "Renderer/Texture.h"
#include "VulkanStructures.h"

namespace yoyo 
{
    class VulkanTexture : public Texture
    {
    public:
        AllocatedImage allocated_image;
        VkImageView image_view;
        VkSampler sampler;

        virtual void UploadTextureData(bool free_host_memory = false) override;

        // Returns true if vulkan texture has been initialized
        const bool IsInitialized() const;
    };
}