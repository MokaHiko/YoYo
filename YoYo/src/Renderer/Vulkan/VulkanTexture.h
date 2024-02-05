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

        virtual void UploadTextureData(bool free_host_memory = false) override;
    };
}