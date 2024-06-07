#pragma once

#include "Defines.h"
#include "Core/Memory.h"

#include "Resource/Resource.h"
#include "RenderTypes.h"

namespace yoyo
{
    // TODO: Abstract to general ShaderPass
    struct VulkanShaderPass;

    struct ShaderInput
    {
        std::string name;
        Format format;
        uint32_t offset = 0; // offset into input in bytes
        uint32_t location = -1;
    };

    // Default shaders that are guaranteed to be implemented
    constexpr char* DEFAULT_WIREFRAME_SHADER_NAME = "unlit_wireframe_shader";
    constexpr char* DEFAULT_UNLIT_SHADER_NAME = "unlit_shader";
    constexpr char* DEFAULT_LINE_SHADER_NAME = "lit_line_shader";
    constexpr char* DEFAULT_POINT_SHADER_NAME = "lit_point_shader";

    class YAPI Shader : public Resource
    {
    public:
        RESOURCE_TYPE(Shader)

        std::unordered_map<MeshPassType, Ref<VulkanShaderPass>> shader_passes;
        static Ref<Shader> Create(const std::string& name = "", bool instanced = false);

        bool instanced = false;
    protected:
        Shader() = default;
        virtual ~Shader() = default;
    };
}