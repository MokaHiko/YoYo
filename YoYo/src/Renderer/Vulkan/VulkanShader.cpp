#include "VulkanShader.h"

#include "Core/Log.h"
#include <spirv_reflect.h>

#include "Platform/Platform.h"

namespace yoyo
{
    void ParseDescriptorSetsFromSpirV(const void* spirv_code, size_t spirv_nbytes, VkShaderStageFlagBits stage, std::vector<VulkanDescriptorSetInformation>& set_infos)
	{
		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(spirv_nbytes, spirv_code, &module);
		YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

		uint32_t count = 0;
		result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectDescriptorSet*> r_descriptor_sets(count);
		result = spvReflectEnumerateDescriptorSets(&module, &count, r_descriptor_sets.data());
		YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

		for (uint32_t i = 0; i < r_descriptor_sets.size(); i++)
		{
			const SpvReflectDescriptorSet& r_descriptor_set = *r_descriptor_sets[i];
			YINFO("\tDescriptor Set %d:", r_descriptor_set.set);

			// Check if descriptor set already exists
			auto it = std::find_if(set_infos.begin(), set_infos.end(), [&](const VulkanDescriptorSetInformation& ds) {return ds.index == r_descriptor_set.set;});
			if (it != set_infos.end())
			{
				for (uint32_t j = 0; j < r_descriptor_set.binding_count; j++)
				{
					const SpvReflectDescriptorBinding& r_descriptor_binding = *r_descriptor_set.bindings[j];
					VkDescriptorType type = static_cast<VkDescriptorType>(r_descriptor_binding.descriptor_type);

					VulkanBinding binding = {};
					binding.type = type;

					it->AddBinding(r_descriptor_binding.binding, stage, binding);
					
					const char* binding_name = *r_descriptor_binding.name != 0? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;
					YINFO("\t\tbinding %d: %s | type: %d", r_descriptor_binding.binding, binding_name, r_descriptor_binding.descriptor_type);
				};
			}
			else
			{
				VulkanDescriptorSetInformation descriptor_set = {};
				descriptor_set.shader_stage = stage;
				descriptor_set.index = r_descriptor_set.set;

				for (uint32_t j = 0; j < r_descriptor_set.binding_count; j++)
				{
					const SpvReflectDescriptorBinding& r_descriptor_binding = *r_descriptor_set.bindings[j];
					VkDescriptorType type = static_cast<VkDescriptorType>(r_descriptor_binding.descriptor_type);

					VulkanBinding binding = {};
					binding.type = type;

					// Binding data
					for(int i = 0; i < r_descriptor_binding.type_description->member_count; i++)
					{
						const SpvReflectTypeDescription& type_desc = r_descriptor_binding.type_description->members[i];

						VulkanBinding::BindingProperty binding_prop;
						binding_prop.offset = binding.Size();

						switch(r_descriptor_binding.type_description->members[i].op)
						{
							case(SpvOpTypeFloat):
							{
								binding_prop.size = type_desc.traits.numeric.scalar.width / 8;
							}break;

							case(SpvOpTypeMatrix):
							{
								binding_prop.size = type_desc.traits.numeric.matrix.column_count * type_desc.traits.numeric.matrix.column_count * (type_desc.traits.numeric.scalar.width / 8);
							}break;

							case(SpvOpTypeVector):
							{
								binding_prop.size = type_desc.traits.numeric.vector.component_count * (type_desc.traits.numeric.scalar.width / 8);
							}break;

							case(SpvOpTypeSampler):
							{
								binding_prop.size = 0;
							}break;

							default:
							{
								YINFO("\t\t\t Uknown Type");
							}break;
						}

						binding.AddProperty(type_desc.struct_member_name, binding_prop);
					}

					descriptor_set.AddBinding(r_descriptor_binding.binding, stage, binding);
					const char* binding_name = *r_descriptor_binding.name != 0? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;
					YINFO("\t\tbinding %d: %s | type: %d", r_descriptor_binding.binding, binding_name, r_descriptor_binding.descriptor_type);
				};

				set_infos.push_back(descriptor_set);
			}
		}

		spvReflectDestroyShaderModule(&module);
	}

    VulkanShader::VulkanShader()
    {
    }

    VulkanShader::~VulkanShader()
    {
    } 

	void VulkanShaderEffect::PushShader(Ref<VulkanShaderModule> shader_module, VkShaderStageFlagBits stage)
	{
		YINFO("%s:", shader_module->source_path.c_str());
        ParseDescriptorSetsFromSpirV(shader_module->code.data(), shader_module->code.size() * sizeof(uint32_t), stage, set_infos);
        stages.push_back({shader_module, stage});
	}
}