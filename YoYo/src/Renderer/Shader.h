#pragma once

#include "Defines.h"
#include "Core/Memory.h"

namespace yoyo
{
    enum class MeshPassType
    {
        FORWARD,
        FORWARD_TRANSPARENT,
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
        // TODO: change uint64_t to resource type to support 32 bit
        uint64_t m_id; // Id of this resource
    };
}