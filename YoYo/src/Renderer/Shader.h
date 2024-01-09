#pragma once

#include "Defines.h"

namespace yoyo
{
    struct YAPI ShaderParameter
    {
        void* data;
        std::string name;
    };

    class YAPI Shader
    {
    public:
        Shader();
        ~Shader();

        virtual void Bind() {};
        virtual void Unbind() {};

        std::vector<ShaderParameter> parameters;
    private:
        // TODO: change uint64_t to resource type to support 32 bit
        uint64_t m_id; // Id of this resource
    };
}