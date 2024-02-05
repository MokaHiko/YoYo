#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Resource/Resource.h"

namespace yoyo
{
    enum class MeshPassType
    {
        Forward,
        ForwardTransparent,
        Shadow,
        PostProcess,
    };

    // TODO: Abstract to general ShaderPass
    struct VulkanShaderPass;

    class YAPI Shader
    {
    public:
        std::unordered_map<MeshPassType, Ref<VulkanShaderPass>> shader_passes;
        const ResourceId ID() const {return m_id;}

        static Ref<Shader> Create(const std::string& name = "");
    protected:
        Shader();
        virtual ~Shader();

        ResourceId m_id; // Id of this resource
    };
}