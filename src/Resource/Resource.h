#pragma once

#include "Platform/Platform.h"

namespace yoyo
{
    // The unique identifier of resource instance
    using ResourceId = uint64_t;
    constexpr ResourceId INVALID_RESOURCE_ID = -1;

    // The unique identifier of resource type
    using ResourceType = uint64_t;

    // ex. some_folder/my_file.extension => my_file.extension
    const std::string FileNameFromFullPath(const std::string &full_path);

// #define RESOURCE_TYPE(type) const ResourceType Type() const override {return s_resource_type;}\
//                                 inline static const ResourceType s_resource_type = Platform::GenerateUUIDV4();\
//                                 inline static const std::string s_resource_type_name = #type;\
//                                 inline static const std::string TypeName() {return s_resource_type_name;}\

#define RESOURCE_TYPE(type) \
    virtual const ResourceType Type() const override {return s_resource_type;} \
    inline static const ResourceType s_resource_type = Platform::GenerateUUIDV4(); \
    inline static constexpr const char* s_resource_type_name = #type; \
    inline static constexpr const char* TypeName() { return s_resource_type_name; }
                                
    // Base class for all resources in the engine
    class YAPI Resource
    {
    public:
        Resource() = default;
        virtual ~Resource() = default;

        const ResourceId Id() const { return m_id; }
        virtual const ResourceType Type() const = 0;

        std::string name;
    private:
        friend class ResourceManager;
        ResourceId m_id = INVALID_RESOURCE_ID;
    };
}