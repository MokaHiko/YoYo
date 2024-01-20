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
    };
}