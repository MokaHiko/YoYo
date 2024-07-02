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

static const char *MATERIAL_TEXTURES_DESCRIPTOR_NAME = "material_textures";

namespace yoyo
{
    VulkanMaterialSystem::VulkanMaterialSystem()
        : m_deletion_queue(),
          m_device(VK_NULL_HANDLE),          // Assuming m_device is of type VkDevice
          m_linear_sampler(VK_NULL_HANDLE),  // Assuming m_linear_sampler is of type VkSampler
          m_nearest_sampler(VK_NULL_HANDLE), // Assuming m_nearest_sampler is of type VkSampler
          m_renderer(nullptr)
    {
    }

    VulkanMaterialSystem::~VulkanMaterialSystem()
    {
    }

    void VulkanMaterialSystem::Init(VulkanRenderer *renderer)
    {
        m_renderer = renderer;

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

        renderer->PhysicalDeviceProperties().limits.maxPerStageDescriptorSamplers;

        m_deletion_queue->Push([=]()
                               {
            vkDestroySampler(m_device, m_nearest_sampler, nullptr);
            vkDestroySampler(m_device, m_linear_sampler, nullptr); });

        EventManager::Instance().Subscribe(MaterialCreatedEvent::s_event_type, [&](Ref<Event> event)
                                           {
            auto material_created_event = std::static_pointer_cast<MaterialCreatedEvent>(event);
            return RegisterMaterial(std::static_pointer_cast<VulkanMaterial>(material_created_event->material)); });
    }

    void VulkanMaterialSystem::Shutdown() {}

    bool VulkanMaterialSystem::RegisterMaterial(Ref<VulkanMaterial> material)
    {
        for (auto pass_it = material->shader->shader_passes.begin(); pass_it != material->shader->shader_passes.end(); pass_it++)
        {
            // Find descriptor sets info for shader pass
            std::vector<VulkanDescriptorSetInformation> &shader_pass_sets = (*(pass_it->second)).effect->set_infos;

            MeshPassType mesh_pass_type = pass_it->first;
            uint32_t mesh_pass_descriptor_set_count = static_cast<uint32_t>(shader_pass_sets.size());

            // Resize descriptors for each shader pass
            material->descriptors[mesh_pass_type].resize(mesh_pass_descriptor_set_count);

            for (const VulkanDescriptorSetInformation &set : shader_pass_sets)
            {
                for (auto &it = set.bindings.begin(); it != set.bindings.end(); it++)
                {
                    // Custom material properties
                    if (it->second.name == "Material")
                    {
                        // TODO: Clean this re direction
                        VulkanDescriptorSetInformation &material_property_set_info = material->descriptors[MeshPassType::Forward][set.index].info;
                        material_property_set_info = set;

                        // Copy binding info to material properties
                        for (auto binding_it = material_property_set_info.bindings.begin(); binding_it != material_property_set_info.bindings.end(); binding_it++)
                        {
                            VulkanBinding &binding = binding_it->second;
                            for (auto binding_prop_it = binding.Properties().begin(); binding_prop_it != binding.Properties().end(); binding_prop_it++)
                            {
                                const std::string &binding_prop_name = binding_prop_it->first;
                                const VulkanBinding::BindingProperty &binding_prop = binding_prop_it->second;

                                MaterialProperty material_prop = {};
                                material_prop.size = binding_prop.size;
                                material_prop.offset = binding_prop.offset;
                                material_prop.type = (MaterialPropertyType)binding_prop.type;

                                material->AddProperty(binding_prop_name, material_prop);
                            }
                        }

                        // Custom material properties
                        VulkanBinding binding = material->descriptors[MeshPassType::Forward][set.index].info.bindings[0];
                        uint32_t binding_index = 0;

                        size_t padded_material_property_size = VulkanResourceManager::PadToUniformBufferSize(binding.Size());
                        material->m_properties_buffer = VulkanResourceManager::CreateBuffer<>(padded_material_property_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                        // 0 out property buffer
                        void *property_data;
                        VulkanResourceManager::MapMemory(material->m_properties_buffer.allocation, &property_data);
                        memset(property_data, 0, material->PropertyDataSize());
                        VulkanResourceManager::UnmapMemory(material->m_properties_buffer.allocation);

                        VkDescriptorBufferInfo property_buffer_info = {};
                        property_buffer_info.offset = 0;
                        property_buffer_info.buffer = material->m_properties_buffer.buffer;
                        property_buffer_info.range = VK_WHOLE_SIZE;

                        VulkanDescriptorSet &property_set = material->descriptors[MeshPassType::Forward][set.index];
                        VkDescriptorSetLayout property_set_layout = {};

                        DescriptorBuilder builder = DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get());
                        builder.BindBuffer(binding_index, &property_buffer_info, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, material_property_set_info.shader_stage);
                        builder.Build(&property_set.set, &property_set_layout);

                        material->descriptors[MeshPassType::Forward][set.index].set = property_set.set;
                    };
                }
            }
        }

        return true;
    }

    Ref<VulkanShaderPass> VulkanMaterialSystem::CreateShaderPass(VkRenderPass render_pass, Ref<VulkanShaderEffect> effect, bool offscreen)
    {
        Ref<VulkanShaderPass> shader_pass = CreateRef<VulkanShaderPass>();
        shader_pass->effect = effect;

        PipelineBuilder builder = {};
        for (VulkanShaderEffect::ShaderStage &stage : effect->stages)
        {
            builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(stage.stage, stage.module->module));
        }

        // Check if vertex shader
        auto it = std::find_if(effect->stages.begin(), effect->stages.end(), [&](const VulkanShaderEffect::ShaderStage &stage)
                               { return stage.stage == VK_SHADER_STAGE_VERTEX_BIT; });
        YASSERT(it != effect->stages.end(), "Shader Effect has not vertex shader!");

        std::vector<VkVertexInputAttributeDescription> vertex_attrib_descs;
        std::vector<VkVertexInputBindingDescription> vertex_binding_descs;

        const VulkanShaderEffect::ShaderStage &vertex_shader_stage = *it;
        vertex_attrib_descs = GenerateVertexAttributeDescriptions(vertex_shader_stage.inputs);
        vertex_binding_descs = GenerateVertexBindingDescriptions(vertex_shader_stage.inputs, VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX);

        builder.vertex_input_info = vkinit::PipelineVertexInputStateCreateInfo();

        builder.vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attrib_descs.size());
        builder.vertex_input_info.pVertexAttributeDescriptions = vertex_attrib_descs.data();

        builder.vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_binding_descs.size());
        builder.vertex_input_info.pVertexBindingDescriptions = vertex_binding_descs.data();

        builder.input_assembly = vkinit::PipelineInputAssemblyStateCreateInfo(effect->primitive_topology);

        builder.rasterizer = vkinit::PipelineRasterizationStateCreateInfo(effect->polygon_mode, effect->line_width);
        builder.multisampling = vkinit::PipelineMultisampleStateCreateInfo();
        builder.depth_stencil = vkinit::PipelineDepthStencilStateCreateInfo(effect->depth_test_enabled,
                                                                            effect->depth_write_enabled,
                                                                            effect->depth_compare_op,
                                                                            effect->stencil_test_enabled,
                                                                            effect->stencil_fail_op,
                                                                            effect->stencil_pass_op,
                                                                            effect->stencil_depth_fail_op,
                                                                            effect->stencil_compare_op,
                                                                            effect->stencil_compare_mask,
                                                                            effect->stencil_write_mask,
                                                                            effect->stencil_reference);
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
        for (VulkanDescriptorSetInformation &descriptor : effect->set_infos)
        {
            DescriptorBuilder builder = DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get());
            for (auto it = descriptor.bindings.begin(); it != descriptor.bindings.end(); it++)
            {
                uint32_t binding_index = it->first;
                VulkanBinding &binding = it->second;

                if (binding.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || binding.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    builder.BindBuffer(binding_index, nullptr, binding.type, descriptor.shader_stage);
                }
                else if (binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                {
                    const VulkanBinding::BindingProperty &property = binding.Properties().begin()->second;
                    switch (property.type)
                    {
                    case (VulkanBindingPropertyType::Texture2D):
                    case (VulkanBindingPropertyType::TextureCube):
                    {
                        builder.BindImage(binding_index, nullptr, binding.type, descriptor.shader_stage);
                    }
                    break;

                    case (VulkanBindingPropertyType::Texture2DArray):
                    case (VulkanBindingPropertyType::TextureCubeArray):
                    {
                        builder.BindImageArray(binding_index, nullptr, property.count, binding.type, descriptor.shader_stage);
                    }
                    break;

                    default:
                    {
                        YERROR("Uknown image sampler binding type!");
                    }
                    break;
                    }
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
        builder.viewport.width = static_cast<float>(m_renderer->Settings().width);
        builder.viewport.height = static_cast<float>(m_renderer->Settings().height);

        builder.viewport.minDepth = 0;
        builder.viewport.maxDepth = 1.0f;

        builder.scissor = {};
        builder.scissor.extent = {m_renderer->Settings().width, m_renderer->Settings().height};
        builder.scissor.offset = {0, 0};

        // TODO: Cache Shader passes (pipelines)
        shader_pass->pipeline = builder.Build(m_device, render_pass, offscreen);

        m_deletion_queue->Push([=]()
                               {
            vkDestroyPipelineLayout(m_device, shader_pass->layout, nullptr);
            vkDestroyPipeline(m_device, shader_pass->pipeline, nullptr); });

        return shader_pass;
    }

    void VulkanMaterialSystem::UpdateMaterial(Ref<VulkanMaterial> material)
    {
        uint32_t texture_count = static_cast<uint32_t>(material->textures.size());
        VkDescriptorSet material_textures_set = {};
        VkDescriptorSetLayout material_textures_set_layout = {};
        std::vector<VkDescriptorImageInfo> material_textures_infos = {};

        if ((material->DirtyFlags() & MaterialDirtyFlags::Texture) == MaterialDirtyFlags::Texture)
        {
            for (Ref<Texture> texture : material->textures)
            {
                Ref<VulkanTexture> t = std::static_pointer_cast<VulkanTexture>(texture);

                VkDescriptorImageInfo material_texture_info = {};
                material_texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                material_texture_info.imageView = t->image_view;
                material_texture_info.sampler = t->sampler;

                material_textures_infos.push_back(material_texture_info);
            }
        }

        for (auto it = material->shader->shader_passes.begin(); it != material->shader->shader_passes.end(); it++)
        {
            const Ref<VulkanShaderPass> &shader_pass = it->second;
            const MeshPassType shader_pass_type = it->first;

            for (VulkanDescriptorSetInformation &descriptor_info : shader_pass->effect->set_infos)
            {
                DescriptorBuilder builder = DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get());
                for (auto it = descriptor_info.bindings.begin(); it != descriptor_info.bindings.end(); it++)
                {
                    uint32_t binding_index = it->first;
                    VulkanBinding &binding = it->second;

                    switch (binding.type)
                    {
                    case (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER):
                    {
                    }
                    break;

                    case (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER):
                    {
                    }
                    break;

                    case (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER):
                    {
                        if ((material->DirtyFlags() & MaterialDirtyFlags::Texture) == MaterialDirtyFlags::Texture)
                        {
                            if (binding.name != MATERIAL_TEXTURES_DESCRIPTOR_NAME)
                            {
                                continue;
                            }

                            const VulkanBinding::BindingProperty &property = binding.Properties().begin()->second;

                            switch (property.type)
                            {
                            case (VulkanBindingPropertyType::Texture2DArray):
                            {
                                // Insert blank textures if not provided in material
                                if (texture_count < property.count)
                                {
                                    for (int i = 0; i < (property.count - texture_count); i++)
                                    {
                                        Ref<VulkanTexture> blank_texture = std::static_pointer_cast<VulkanTexture>(ResourceManager::Instance().Load<Texture>("white"));
                                        YASSERT(blank_texture != nullptr, "Invalid blank texture!");

                                        VkDescriptorImageInfo texture_info = {};
                                        texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                        texture_info.imageView = blank_texture->image_view;
                                        texture_info.sampler = blank_texture->sampler;

                                        material_textures_infos.push_back(texture_info);
                                    }
                                }

                                DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
                                    .BindImageArray(binding_index, material_textures_infos.data(), property.count, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_info.shader_stage)
                                    .Build(&material_textures_set, &material_textures_set_layout);

                                material->descriptors[shader_pass_type][descriptor_info.index].set = material_textures_set;
                            }
                            break;

                            case (VulkanBindingPropertyType::Texture2D):
                            case (VulkanBindingPropertyType::TextureCube):
                            {
                                VkDescriptorSet texture_set = {};
                                VkDescriptorSetLayout texture_set_layout = {};
                                DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
                                    .BindImage(binding_index, material_textures_infos.data(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_info.shader_stage)
                                    .Build(&texture_set, &texture_set_layout);

                                material->descriptors[shader_pass_type][descriptor_info.index].set = texture_set;
                            }
                            break;

                            default:
                            {
                                YERROR("Uknown material texture binding!");
                            }
                            break;
                            }
                        }
                    }
                    break;

                    default:
                    {
                        YERROR("Uknown binding type at descriptor set: %u binding: %u", descriptor_info.index, binding_index);
                    }
                    break;
                    }
                }
            }
        }

        // Update custom properties
        if ((material->DirtyFlags() & MaterialDirtyFlags::Property) == MaterialDirtyFlags::Property)
        {
            void *property_data;
            VulkanResourceManager::MapMemory(material->m_properties_buffer.allocation, &property_data);
            memcpy(property_data, material->PropertyData(), material->PropertyDataSize());
            VulkanResourceManager::UnmapMemory(material->m_properties_buffer.allocation);
        }

        material->DirtyFlags() = MaterialDirtyFlags::Clean;
    }
}