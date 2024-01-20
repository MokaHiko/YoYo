#pragma once

#include "Defines.h"
#include "Core/Memory.h"

namespace yoyo
{
    enum class MeshPassType
    {
        Forward,
        Forward_transparent,
        Shadow,
    };

    // TODO: Abstract to general ShaderPass
    struct VulkanShaderPass;

    class YAPI Shader
    {
    public:
        Shader();
        ~Shader();

        std::unordered_map<MeshPassType, Ref<VulkanShaderPass>> shader_passes;
    private:
        uint64_t m_id; // Id of this resource
    };
}