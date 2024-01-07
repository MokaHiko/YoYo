#include "VulkanShader.h"

#include "Core/Log.h"
#include <spirv_reflect.h>

namespace yoyo
{
    VulkanShader::VulkanShader()
    {
    }

    VulkanShader::~VulkanShader()
    {
    }

    void ParseDescriptorSetFromSpirV(const void *spirv_code, size_t spirv_nbytes, VkShaderStageFlagBits stage)
    {
        // // Generate reflection data for a shader
        // SpvReflectShaderModule module;
        // SpvReflectResult result = spvReflectCreateShaderModule(spirv_nbytes, spirv_code, &module);
        // YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        // // Get descriptor bindings
        // uint32_t count = 0;
        // result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
        // YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        // std::vector<SpvReflectDescriptorSet *> r_descriptor_sets(count);
        // result = spvReflectEnumerateDescriptorSets(&module, &count, r_descriptor_sets.data());
        // YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        // for (uint32_t i = 0; i < r_descriptor_sets.size(); i++)
        // {
        //     YINFO("Descriptor Set: %d", i);
        //     const SpvReflectDescriptorSet &r_descriptor_set = *r_descriptor_sets[i];

        //     // Check if descriptor set already exists
        //     auto it = std::find_if(_required_descriptors.begin(), _required_descriptors.end(), [&](const VulkanDescriptorSet &ds)
        //                            { return ds.set == r_descriptor_set.set; });

        //     if (it != _required_descriptors.end())
        //     {
        //         for (uint32_t j = 0; j < r_descriptor_set.binding_count; j++)
        //         {
        //             const SpvReflectDescriptorBinding &r_descriptor_binding = *r_descriptor_set.bindings[j];
        //             VkDescriptorType type = static_cast<VkDescriptorType>(r_descriptor_binding.descriptor_type);

        //             YINFO("binding: %d, descriptor type: %d", r_descriptor_binding.binding, r_descriptor_binding.descriptor_type);
        //             it->AddBinding(r_descriptor_binding.binding, type, stage);
        //         };
        //     }
        //     else
        //     {
        //         VulkanDescriptorSet descriptor_set = {};
        //         descriptor_set.shader_stage = stage;
        //         descriptor_set.set = r_descriptor_set.set;

        //         for (uint32_t j = 0; j < r_descriptor_set.binding_count; j++)
        //         {
        //             const SpvReflectDescriptorBinding &r_descriptor_binding = *r_descriptor_set.bindings[j];
        //             VkDescriptorType type = static_cast<VkDescriptorType>(r_descriptor_binding.descriptor_type);

        //             YINFO("binding: %d, descriptor type: %d", r_descriptor_binding.binding, r_descriptor_binding.descriptor_type);
        //             descriptor_set.AddBinding(r_descriptor_binding.binding, type, stage);
        //         };

        //         _required_descriptors.push_back(descriptor_set);
        //     }
        // }

        // // Clean up
        // spvReflectDestroyShaderModule(&module);
    }
}