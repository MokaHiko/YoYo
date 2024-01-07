#pragma once

#include "Renderer/Texture.h"

namespace yoyo 
{
    class VulkanTexture : public Texture
    {
    public:
        
    private:
        AllocatedImage allocated_image;
    }
}