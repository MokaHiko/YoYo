#include "VulkanShader.h"

#include "Core/Assert.h"
#include "Core/Log.h"
#include <spirv_reflect.h>

#include "Platform/Platform.h"
#include "Resource/ResourceEvent.h"

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
					binding.name = *r_descriptor_binding.name != 0? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;

					it->AddBinding(r_descriptor_binding.binding, stage, binding);
					
					const char* binding_name = *r_descriptor_binding.name != 0? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;
					//YINFO("\t\tbinding %d: %s | type: %d", r_descriptor_binding.binding, binding_name, r_descriptor_binding.descriptor_type);
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

						VulkanBinding::BindingProperty binding_prop = {};
						binding_prop.offset = binding.Size();
						binding.name = *r_descriptor_binding.name != 0? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;

						switch(r_descriptor_binding.type_description->members[i].op)
						{
							case(SpvOpTypeFloat):
							{
								binding_prop.size = type_desc.traits.numeric.scalar.width / 8;
								binding_prop.type = VulkanBindingPropertType::Float32;
							}break;

							case(SpvOpTypeMatrix):
							{
								binding_prop.size = type_desc.traits.numeric.matrix.column_count * type_desc.traits.numeric.matrix.column_count * (type_desc.traits.numeric.scalar.width / 8);
							}break;

							case(SpvOpTypeVector):
							{
								binding_prop.size = type_desc.traits.numeric.vector.component_count * (type_desc.traits.numeric.scalar.width / 8);

								// TODO: Check if vec3 
								binding_prop.type = VulkanBindingPropertType::Vec4;
							}break;

							case(SpvOpTypeSampler):
							{
								binding_prop.size = 0;
							}break;

							case(SpvOpTypeRuntimeArray):
							{
								binding_prop.size = 0;
							}break;

							case(SpvOpTypeInt):
							{
								binding_prop.size = type_desc.traits.numeric.scalar.width / 8;
							}break;

							default:
							{
								//YINFO("\t\t\t Binding Type Uknown");
							}break;
						}

						binding.AddProperty(type_desc.struct_member_name, binding_prop);
					}

					descriptor_set.AddBinding(r_descriptor_binding.binding, stage, binding);
					const char* binding_name = *r_descriptor_binding.name != 0? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;
					//YINFO("\t\tbinding %d: %s | type: %d", r_descriptor_binding.binding, binding_name, r_descriptor_binding.descriptor_type);
				};

				set_infos.push_back(descriptor_set);
			}
		}

		spvReflectDestroyShaderModule(&module);
	}

	static void ParseSpirVStage(const Ref<VulkanShaderModule>& shader_module, VulkanShaderEffect& effect, VkShaderStageFlagBits& stage)
	{
		const void* spirv_code = shader_module->code.data();
		size_t spirv_nbytes = shader_module->code.size() * sizeof(uint32_t);

		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(spirv_nbytes, spirv_code, &module);
		YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

		uint32_t count = 0;
		result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectDescriptorSet*> r_descriptor_sets(count);
		result = spvReflectEnumerateDescriptorSets(&module, &count, r_descriptor_sets.data());
		YASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
		
		// Shader stage
		VulkanShaderEffect::ShaderStage shader_stage = {};
		shader_stage.stage = stage;
		shader_stage.module = shader_module;

		// Inputs Variables
		YTRACE("\tInputs :");
		{
			uint32_t var_count = 0;
			result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
			assert(result == SPV_REFLECT_RESULT_SUCCESS);
			SpvReflectInterfaceVariable** input_vars = (SpvReflectInterfaceVariable**)YAllocate(var_count * sizeof(SpvReflectInterfaceVariable*));
			result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);
			assert(result == SPV_REFLECT_RESULT_SUCCESS);

			shader_stage.inputs.resize(var_count);

			VkVertexInputAttributeDescription input_description = {};
			for (uint32_t i = 0; i < var_count; i++)
			{
				SpvReflectInterfaceVariable* input_var = input_vars[i];
				if (input_var->built_in > 0)
				{
					continue;
				}

				ShaderInput input = {};
				input.name = input_var->name;
				input.format = (Format)input_var->format;
				input.offset = input_var->word_offset.location;
				input.location = input_var->location;
				shader_stage.inputs[input_var->location] = input;

				YTRACE("\t\t%s: ", input_var->name);
				YTRACE("\t\t\tlocation: %d ", input_var->location);
				YTRACE("\t\t\tformat: %d ", input_var->format);
				YTRACE("\t\t\toffset: %u ", input_var->word_offset.location);
			}
			shader_stage.inputs.shrink_to_fit();

			YFree(input_vars);
		} 
		effect.stages.push_back(shader_stage);

		// Descriptors 
		for (uint32_t i = 0; i < r_descriptor_sets.size(); i++)
		{
			const SpvReflectDescriptorSet& r_descriptor_set = *r_descriptor_sets[i];
			//YINFO("\tDescriptor Set %d:", r_descriptor_set.set);

			// Check if descriptor set already exists
			auto it = std::find_if(effect.set_infos.begin(), effect.set_infos.end(), [&](const VulkanDescriptorSetInformation& ds) {return ds.index == r_descriptor_set.set;});
			if (it != effect.set_infos.end())
			{
				for (uint32_t j = 0; j < r_descriptor_set.binding_count; j++)
				{
					const SpvReflectDescriptorBinding& r_descriptor_binding = *r_descriptor_set.bindings[j];
					VkDescriptorType type = static_cast<VkDescriptorType>(r_descriptor_binding.descriptor_type);

					VulkanBinding binding = {};
					binding.type = type;
					binding.name = *r_descriptor_binding.name != 0? std::string(r_descriptor_binding.name) : std::string(r_descriptor_binding.type_description->type_name);

					it->AddBinding(r_descriptor_binding.binding, stage, binding);
					YTRACE("\t\tds: %d | binding %d: %s | type: %d", r_descriptor_set.set, r_descriptor_binding.binding, binding.name.c_str(), r_descriptor_binding.descriptor_type);
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
					binding.name = *r_descriptor_binding.name != 0? std::string(r_descriptor_binding.name) : std::string(r_descriptor_binding.type_description->type_name);

					// Binding data
					for (int i = 0; i < r_descriptor_binding.type_description->member_count; i++)
					{
						const SpvReflectTypeDescription& type_desc = r_descriptor_binding.type_description->members[i];

						VulkanBinding::BindingProperty binding_prop;
						binding_prop.offset = binding.Size();

						switch (r_descriptor_binding.type_description->members[i].op)
						{
						case(SpvOpTypeFloat):
						{
							binding_prop.size = type_desc.traits.numeric.scalar.width / 8;
							binding_prop.type = VulkanBindingPropertType::Float32;
						}break;

						case(SpvOpTypeMatrix):
						{
							binding_prop.size = type_desc.traits.numeric.matrix.column_count * type_desc.traits.numeric.matrix.column_count * (type_desc.traits.numeric.scalar.width / 8);
						}break;

						case(SpvOpTypeVector):
						{
							binding_prop.size = type_desc.traits.numeric.vector.component_count * (type_desc.traits.numeric.scalar.width / 8);

							// TODO: Check if vec3 
							binding_prop.type = VulkanBindingPropertType::Vec4;
						}break;

						case(SpvOpTypeSampler):
						{
							binding_prop.size = 0;
						}break;

						case(SpvOpTypeRuntimeArray):
						{
							binding_prop.size = 0;
						}break;

						case(SpvOpTypeInt):
						{
							binding_prop.size = type_desc.traits.numeric.scalar.width / 8;
							binding_prop.type = VulkanBindingPropertType::Int32;
						}break;

						default:
						{
							//YINFO("\t\t\t Binding Type Uknown");
						}break;
						}

						binding.AddProperty(type_desc.struct_member_name, binding_prop);
					}

					descriptor_set.AddBinding(r_descriptor_binding.binding, stage, binding);
					const char* binding_name = *r_descriptor_binding.name != 0 ? r_descriptor_binding.name : r_descriptor_binding.type_description->type_name;
					YTRACE("\t\tds: %d | binding %d: %s | type: %d", descriptor_set.index, r_descriptor_binding.binding, binding_name, r_descriptor_binding.descriptor_type);
				};

				effect.set_infos.push_back(descriptor_set);
			}
		}

		spvReflectDestroyShaderModule(&module);
	}

	Ref<Shader> Shader::Create(const std::string& name, bool instanced)
	{
		Ref<VulkanShader> shader = CreateRef<VulkanShader>();
		shader->name = name;
		shader->instanced = instanced;

		EventManager::Instance().Dispatch(CreateRef<ShaderCreatedEvent>(shader));
		return shader;
	}

	VulkanShader::VulkanShader()
	{
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShaderEffect::PushShader(Ref<VulkanShaderModule> shader_module, VkShaderStageFlagBits stage)
	{
		YTRACE("%s:", shader_module->source_path.c_str());

		ParseSpirVStage(shader_module, *this, stage);
	}
}