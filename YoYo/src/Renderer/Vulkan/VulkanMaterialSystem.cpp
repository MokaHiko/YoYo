#include "VulkanMaterialSystem.h"

#include "VulkanMaterial.h"

#include "VulkanUtils.h"
#include "VulkanInitializers.h"

#include "VulkanRenderer.h"

// TODO: Separate Vertex Descriptions
#include "VulkanMesh.h"

namespace yoyo
{
	void VulkanMaterialSystem::Init(VulkanRenderer* renderer)
	{
        m_descriptor_allocator = renderer->DescAllocator();
        m_descriptor_layout_cache = renderer->DescLayoutCache();

		m_device = renderer->Device();
		m_deletion_queue = &renderer->DeletionQueue();
	}

	void VulkanMaterialSystem::Shutdown()
	{
	}

	Ref<Material> VulkanMaterialSystem::CreateMaterial() 
	{
		return Ref<Material>();
	}

	Ref<VulkanShaderPass> VulkanMaterialSystem::CreateShaderPass(VkRenderPass render_pass, Ref<VulkanShaderEffect> effect)
	{
		Ref<VulkanShaderPass> shader_pass = CreateRef<VulkanShaderPass>();
		shader_pass->effect = effect;

 		PipelineBuilder builder = {};
		for(VulkanShaderEffect::ShaderStage& stage : effect->stages)
		{
        	builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(stage.stage, stage.module->module));
		}

        builder.vertex_input_info = vkinit::PipelineVertexInputStateCreateInfo();
        builder.vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributeDescriptions().size());
        builder.vertex_input_info.pVertexAttributeDescriptions = VertexAttributeDescriptions().data();
        builder.vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexBindingDescriptions().size());
        builder.vertex_input_info.pVertexBindingDescriptions = VertexBindingDescriptions().data();

        builder.input_assembly = vkinit::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        builder.rasterizer = vkinit::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);

        builder.multisampling = vkinit::PipelineMultisampleStateCreateInfo();

        builder.color_blend_attachment = vkinit::PipelineColorBlendAttachmentState();

        builder.depth_stencil = vkinit::PipelineDepthStencilStateCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

        builder.tesselation_state = {};

        // Build descriptor set layouts
        std::vector<VkDescriptorSetLayout> descriptor_layouts;
        for (VulkanDescriptorSet& descriptor : effect->sets)
        {
            // Build descriptor set layouts
            DescriptorBuilder builder = DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get());
            for (auto it = descriptor.bindings.begin(); it != descriptor.bindings.end(); it++)
            {
                uint32_t binding = it->first;
                VkDescriptorType type = it->second;
                if (type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    builder.BindBuffer(binding, nullptr, type, descriptor.shader_stage);
                }
                else if (type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                {
                    builder.BindImage(binding, nullptr, type, descriptor.shader_stage);
                }
            }

            builder.Build(nullptr, &descriptor.descriptor_set_layout);
            descriptor_layouts.push_back(descriptor.descriptor_set_layout);
        }

        VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::PipelineLayoutCreateInfo();
        pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_layouts.size());
        pipeline_layout_info.pSetLayouts = descriptor_layouts.data();
        vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &shader_pass->layout);
        builder.pipeline_layout = shader_pass->layout;

        builder.viewport.x = 0;
        builder.viewport.y = 0;
        builder.viewport.width = 720;
        builder.viewport.height = 480;

        builder.viewport.minDepth = 0;
        builder.viewport.maxDepth = 1.0f;

        builder.scissor = {};
        builder.scissor.extent.width = 720;
        builder.scissor.extent.height = 480;
        builder.scissor.offset.x = 0;
        builder.scissor.offset.y = 0;

		// TODO: Cache Shader passes (pipelines)
        shader_pass->pipeline = builder.Build(m_device, render_pass);

        m_deletion_queue->Push([=]()
		{
			vkDestroyPipelineLayout(m_device, shader_pass->layout, nullptr);
			vkDestroyPipeline(m_device, shader_pass->pipeline, nullptr);
		});

		return shader_pass;
	}

} // namespace yoyo