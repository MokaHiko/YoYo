#include "VulkanMaterialSystem.h"

#include "VulkanUtils.h"
#include "VulkanInitializers.h"

#include "VulkanMaterial.h"
#include "VulkanRenderer.h"

#include "VulkanTexture.h"
#include "VulkanShader.h"
#include "VulkanMesh.h"

#include "Resource/ResourceEvent.h"
#include "Resource/ResourceManager.h"

#include "VulkanResourceManager.h"

namespace yoyo
{
    void VulkanMaterialSystem::Init(VulkanRenderer* renderer)
    {
        m_descriptor_allocator = renderer->DescAllocator();
        m_descriptor_layout_cache = renderer->DescLayoutCache();

        m_device = renderer->Device();
        m_deletion_queue = &renderer->DeletionQueue();

        m_linear_sampler = {};
        VkSamplerCreateInfo linear_sampler_info = vkinit::SamplerCreateInfo(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        vkCreateSampler(m_device, &linear_sampler_info, nullptr, &m_linear_sampler);

        m_nearest_sampler = {};
        VkSamplerCreateInfo nearest_sampler_info = vkinit::SamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        vkCreateSampler(m_device, &nearest_sampler_info, nullptr, &m_nearest_sampler);

        m_deletion_queue->Push([=]() {
            vkDestroySampler(m_device, m_nearest_sampler, nullptr);
            vkDestroySampler(m_device, m_linear_sampler, nullptr);
            });

        EventManager::Instance().Subscribe(MaterialCreatedEvent::s_event_type, [&](Ref<Event> event) {
            auto material_created_event = std::static_pointer_cast<MaterialCreatedEvent>(event);
            return RegisterMaterial(std::static_pointer_cast<VulkanMaterial>(material_created_event->material));
            });
    }

    void VulkanMaterialSystem::Shutdown() {}

    bool VulkanMaterialSystem::RegisterMaterial(Ref<VulkanMaterial> material)
    {
        for (auto pass_it = material->shader->shader_passes.begin(); pass_it != material->shader->shader_passes.end(); pass_it++)
        {
            // Create descriptor for public material properties
            std::vector<VulkanDescriptorSetInformation>& shader_pass_sets = (*(pass_it->second)).effect->set_infos;
            auto property_ds = std::find_if(shader_pass_sets.begin(), shader_pass_sets.end(), [=](const VulkanDescriptorSetInformation& set) {
                return set.index == MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX;
                });

            if (property_ds == shader_pass_sets.end())
            {
                continue;
            }

            VulkanDescriptorSetInformation& material_property_set_info = material->descriptors[MeshPassType::Forward][MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX].info;
            material_property_set_info = *property_ds;

            // Copy binding info to material properties
            for (auto binding_it = material_property_set_info.bindings.begin(); binding_it != material_property_set_info.bindings.end(); binding_it++)
            {
                VulkanBinding& binding = binding_it->second;
                for (auto binding_prop_it = binding.Properties().begin(); binding_prop_it != binding.Properties().end(); binding_prop_it++)
                {
                    const std::string& binding_prop_name = binding_prop_it->first;
                    const VulkanBinding::BindingProperty& binding_prop = binding_prop_it->second;

                    MaterialProperty material_prop = {};
                    material_prop.size = binding_prop.size;
                    material_prop.offset = binding_prop.offset;
                    material_prop.type = (MaterialPropertyType)binding_prop.type;

                    material->AddProperty(binding_prop_name, material_prop);
                }
            }

            // TODO: Give default main texture value or send to untextured material
            VkDescriptorSet textures_set = {};
            material->descriptors[MeshPassType::Forward][MATERIAL_TEXTURE_SET_INDEX].set = textures_set;

            // TODO: move to shader pass loop
            {
                VulkanBinding binding = material->descriptors[MeshPassType::Forward][MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX].info.bindings[0];
                uint32_t binding_index = 0;

                size_t padded_material_property_size = VulkanResourceManager::PadToUniformBufferSize(binding.Size());
                material->m_properties_buffer = VulkanResourceManager::CreateBuffer(padded_material_property_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                // 0 out property buffer
                void* property_data;
                VulkanResourceManager::MapMemory(material->m_properties_buffer.allocation, &property_data);
                memset(property_data, 0, material->PropertyDataSize());
                VulkanResourceManager::UnmapMemory(material->m_properties_buffer.allocation);

                VkDescriptorBufferInfo property_buffer_info = {};
                property_buffer_info.offset = 0;
                property_buffer_info.buffer = material->m_properties_buffer.buffer;
                property_buffer_info.range = VK_WHOLE_SIZE;

                VulkanDescriptorSet& property_set = material->descriptors[MeshPassType::Forward][MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX];
                VkDescriptorSetLayout property_set_layout;
                DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get()).
                    BindBuffer(binding_index, &property_buffer_info, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, material_property_set_info.shader_stage).
                    Build(&property_set.set, &property_set_layout);

                material->descriptors[MeshPassType::Forward][MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX].set = property_set.set;
            }

            return true;
        }



        return false;
    }

    Ref<VulkanShaderPass> VulkanMaterialSystem::CreateShaderPass(VkRenderPass render_pass, Ref<VulkanShaderEffect> effect, bool offscreen)
    {
        Ref<VulkanShaderPass> shader_pass = CreateRef<VulkanShaderPass>();
        shader_pass->effect = effect;

        PipelineBuilder builder = {};
        for (VulkanShaderEffect::ShaderStage& stage : effect->stages)
        {
            builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(stage.stage, stage.module->module));
        }

        // Check if vertex shader
        auto it = std::find_if(effect->stages.begin(), effect->stages.end(), [&](const VulkanShaderEffect::ShaderStage& stage) {
            return stage.stage == VK_SHADER_STAGE_VERTEX_BIT;
            });
        YASSERT(it != effect->stages.end(), "Shader Effect has not vertex shader!");

        std::vector<VkVertexInputAttributeDescription> vertex_attrib_descs;
        std::vector<VkVertexInputBindingDescription> vertex_binding_descs;

        const VulkanShaderEffect::ShaderStage& vertex_shader_stage = *it;
        vertex_attrib_descs = GenerateVertexAttributeDescriptions(vertex_shader_stage.inputs);
        vertex_binding_descs = GenerateVertexBindingDescriptions(vertex_shader_stage.inputs, VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX);

        builder.vertex_input_info = vkinit::PipelineVertexInputStateCreateInfo();

        builder.vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attrib_descs.size());
        builder.vertex_input_info.pVertexAttributeDescriptions = vertex_attrib_descs.data();

        builder.vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_binding_descs.size());
        builder.vertex_input_info.pVertexBindingDescriptions = vertex_binding_descs.data();

        builder.input_assembly = vkinit::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        builder.rasterizer = vkinit::PipelineRasterizationStateCreateInfo(effect->polygon_mode);
        builder.multisampling = vkinit::PipelineMultisampleStateCreateInfo();
        builder.depth_stencil = vkinit::PipelineDepthStencilStateCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
        builder.color_blend_attachment = vkinit::PipelineColorBlendAttachmentState(effect->blend_enable,
                                                                                   effect->src_blend_factor,
                                                                                   effect->dst_blend_factor,
                                                                                   effect->color_blend_op,
                                                                                   effect->src_alpha_blend_factor,
                                                                                   effect->dst_alpha_blend_factor,
                                                                                   effect->alpha_blend_op);
        builder.tesselation_state = {};

        // Build descriptor set layouts
        std::vector<VkDescriptorSetLayout> descriptor_layouts(effect->set_infos.size());
        for (VulkanDescriptorSetInformation& descriptor : effect->set_infos)
        {
            // Build descriptor set layouts
            DescriptorBuilder builder = DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get());
            for (auto it = descriptor.bindings.begin(); it != descriptor.bindings.end(); it++)
            {
                uint32_t binding_index = it->first;
                const VulkanBinding& binding = it->second;

                if (binding.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || binding.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    builder.BindBuffer(binding_index, nullptr, binding.type, descriptor.shader_stage);
                }
                else if (binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                {
                    builder.BindImage(binding_index, nullptr, binding.type, descriptor.shader_stage);
                }
            }

            builder.Build(nullptr, &descriptor.descriptor_set_layout);
            descriptor_layouts[descriptor.index] = descriptor.descriptor_set_layout;
        }

        VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::PipelineLayoutCreateInfo();
        pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_layouts.size());
        pipeline_layout_info.pSetLayouts = descriptor_layouts.data();
        vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &shader_pass->layout);
        builder.pipeline_layout = shader_pass->layout;

        builder.viewport.x = 0;
        builder.viewport.y = 0;
        builder.viewport.width = 1920;
        builder.viewport.height = 1080;

        builder.viewport.minDepth = 0;
        builder.viewport.maxDepth = 1.0f;

        builder.scissor = {};
        builder.scissor.extent = { 1920, 1080 };
        builder.scissor.offset = { 0,0 };

        // TODO: Cache Shader passes (pipelines)
        shader_pass->pipeline = builder.Build(m_device, render_pass, offscreen);

        m_deletion_queue->Push([=]() {
            vkDestroyPipelineLayout(m_device, shader_pass->layout, nullptr);
            vkDestroyPipeline(m_device, shader_pass->pipeline, nullptr);
            });
        return shader_pass;
    }

    void VulkanMaterialSystem::UpdateMaterial(Ref<VulkanMaterial> material)
    {
        if ((material->DirtyFlags() & MaterialDirtyFlags::Texture) == MaterialDirtyFlags::Texture)
        {
            Ref<VulkanTexture> main_texture = std::static_pointer_cast<VulkanTexture>(material->MainTexture());

            VkDescriptorImageInfo main_texture_info = {};
            main_texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            main_texture_info.imageView = main_texture->image_view;
            main_texture_info.sampler = main_texture->sampler;

            VkDescriptorImageInfo specular_texture_info = {};
            if (material->GetTexture((int)MaterialTextureType::SpecularMap))
            {
                specular_texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                specular_texture_info.imageView = std::static_pointer_cast<VulkanTexture>(material->GetTexture((int)MaterialTextureType::SpecularMap))->image_view;
                specular_texture_info.sampler = m_linear_sampler;
            }
            else
            {
                specular_texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                specular_texture_info.imageView = std::static_pointer_cast<VulkanTexture>(ResourceManager::Instance().Load<Texture>("assets/textures/white.yo"))->image_view;
                specular_texture_info.sampler = m_linear_sampler;
            }

            VkDescriptorSet textures_set = {};
            VkDescriptorSetLayout textures_set_layout = {};

            DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
                .BindImage(MATERIAL_MAIN_TEXTURE_DESCRIPTOR_SET_BINDING, &main_texture_info, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .BindImage(MATERIAL_SPECULAR_TEXTURE_DESCRIPTOR_SET_BINDING, &specular_texture_info, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .Build(&textures_set, &textures_set_layout);

            material->descriptors[MeshPassType::Forward][MATERIAL_TEXTURE_SET_INDEX].set = textures_set;
        }

        if ((material->DirtyFlags() & MaterialDirtyFlags::Property) == MaterialDirtyFlags::Property)
        {
            void* property_data;
            VulkanResourceManager::MapMemory(material->m_properties_buffer.allocation, &property_data);
            memcpy(property_data, material->PropertyData(), material->PropertyDataSize());
            VulkanResourceManager::UnmapMemory(material->m_properties_buffer.allocation);
        }

        material->DirtyFlags() = MaterialDirtyFlags::Clean;
    }
}