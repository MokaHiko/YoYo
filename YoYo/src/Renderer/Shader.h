#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Resource/Resource.h"

#include "Renderer/RenderPass.h"

namespace yoyo
{
    // TODO: Abstract to general ShaderPass
    struct VulkanShaderPass;

    class YAPI Shader : public Resource
    {
    public:
        RESOURCE_TYPE(Shader)

        std::unordered_map<MeshPassType, Ref<VulkanShaderPass>> shader_passes;
        static Ref<Shader> Create(const std::string& name = "", bool instanced = false);

        bool instanced;
    protected:
        Shader() = default;
        virtual ~Shader() = default;
    };
}