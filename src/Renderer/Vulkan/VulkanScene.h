#pragma once

#include "VulkanStructures.h"
#include "Renderer/RenderScene.h"

namespace yoyo
{   
    class VulkanRenderScene : public RenderScene
    {
    public:
        VulkanRenderScene(uint32_t max_objects);
        virtual ~VulkanRenderScene();
    private:
        std::vector<VkDrawIndexedIndirectCommand> m_draw_indirects;
    };
}