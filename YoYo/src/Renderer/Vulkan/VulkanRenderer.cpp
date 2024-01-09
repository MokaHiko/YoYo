#include <VkBootstrap.h>

#include "VulkanRenderer.h"

#include "Platform/Platform.h"
#include "Core/Log.h"

#include "VulkanMesh.h"

namespace yoyo
{
    Ref<Renderer> CreateRenderer()
    {
        return CreateRef<VulkanRenderer>();
    }

    VulkanRenderer::VulkanRenderer()
        : Renderer({ RendererType::VULKAN, 2, true }),
        m_instance(VK_NULL_HANDLE), m_surface(VK_NULL_HANDLE),
        m_device(VK_NULL_HANDLE), m_physical_device(VK_NULL_HANDLE) {}

    VulkanRenderer::~VulkanRenderer() {}

    void VulkanRenderer::Init()
    {
        m_frame_count = 0;
        m_frame_context.resize(m_settings.max_frames_in_flight);

        InitVulkan();

        InitSwapchain();
        InitSwapchainRenderPass();
        InitSwapchainFramebuffers();

        InitCommands();
        InitSyncStructures();

        m_descriptor_allocator = CreateRef<DescriptorAllocator>();
        m_descriptor_allocator->Init(m_device);

        m_descriptor_layout_cache = CreateRef<DescriptorLayoutCache>();
        m_descriptor_layout_cache->Init(m_device);

        m_resource_manager = CreateRef<VulkanResourceManager>();
        m_resource_manager->Init(this);

        InitBlitPipeline();
        blit_screen_mesh = CreateRef<VulkanMesh>();
        blit_screen_mesh->vertices.push_back({-1, 0, 0});
        blit_screen_mesh->vertices.push_back({1, 0, 0});
        blit_screen_mesh->vertices.push_back({0, 1.0, 0});

        m_resource_manager->UploadMesh(blit_screen_mesh);
    }

    void VulkanRenderer::Shutdown()
    {
        m_descriptor_allocator->CleanUp();
        m_descriptor_layout_cache->Clear();

        m_resource_manager->Shutdown();

        m_deletion_queue.Flush();
    }

    bool VulkanRenderer::BeginFrame(const RenderPacket& rp)
    {
        vkWaitForFences(m_device, 1, &m_frame_context[m_frame_count].render_fence, VK_TRUE, 1000000000);
        vkResetFences(m_device, 1, &m_frame_context[m_frame_count].render_fence);

        m_swapchain_index = -1;
        VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_frame_context[m_frame_count].present_semaphore, VK_NULL_HANDLE, &m_swapchain_index));

        VkCommandBufferBeginInfo cmd_begin_info = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkCommandBuffer cmd = m_frame_context[m_frame_count].command_buffer;
        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

        // TODO: Bind shadow pass and draw shadow mesh passes

        // TODO: Bind main render pass and draw mesh passes

        // TODO: Bind post processing render pass and draw post process passes
        return true;
    }

    void VulkanRenderer::EndFrame()
    {
        // Blit offscreen target to swapchain
        VkCommandBuffer cmd = m_frame_context[m_frame_count].command_buffer;

        VkRect2D rect = {};
        rect.offset.x = 0;
        rect.offset.y = 0;
        rect.extent.width = 720;
        rect.extent.height = 480;

        VkClearValue clear_value = {};
        clear_value.color = { 1.0f, 1.0f, 1.0f, 1.0f };

        VkRenderPassBeginInfo renderpass_begin_info = vkinit::RenderPassBeginInfo(m_swapchain_framebuffers[m_swapchain_index], m_swapchain_render_pass, rect, &clear_value, 1);
        vkCmdBeginRenderPass(cmd, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blit_pipeline);

        VulkanRenderContext ctx = {};
        ctx.cmd = cmd;

        blit_screen_mesh->Bind(&ctx);
        vkCmdDraw(cmd, blit_screen_mesh->vertices.size(), 1, 0, 0);

        vkCmdEndRenderPass(cmd);

        VK_CHECK(vkEndCommandBuffer(m_frame_context[m_frame_count].command_buffer));
        VkSubmitInfo submit = {};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pCommandBuffers = &m_frame_context[m_frame_count].command_buffer;
        submit.commandBufferCount = 1;

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &m_frame_context[m_frame_count].present_semaphore;
        submit.pWaitDstStageMask = &waitStage;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &m_frame_context[m_frame_count].render_semaphore;

        VK_CHECK(vkQueueSubmit(m_queues.graphics.queue, 1, &submit, m_frame_context[m_frame_count].render_fence));

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.swapchainCount = 1;
        present_info.pSwapchains = &m_swapchain;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &m_frame_context[m_frame_count].render_semaphore;

        present_info.pImageIndices = &m_swapchain_index;
        VK_CHECK(vkQueuePresentKHR(m_queues.graphics.queue, &present_info));

        m_frame_count = (m_frame_count + 1) % m_settings.max_frames_in_flight;
    }

    void VulkanRenderer::InitVulkan()
    {
        vkb::InstanceBuilder builder;
        auto inst_ret = builder.set_app_name("Example Vulkan Application")
            .request_validation_layers()
            .use_default_debug_messenger()
            .build();
        if (!inst_ret)
        {
            YERROR("Failed to create Vulkan instance. Error: ", inst_ret.error().message().c_str());
            return;
        }
        vkb::Instance vkb_inst = inst_ret.value();
        m_instance = vkb_inst.instance;

        Platform::CreateSurface(&m_instance, &m_surface);
        if (!m_surface)
        {
            YERROR("Failed to create Vulkan surface");
            return;
        }

        vkb::PhysicalDeviceSelector selector{ vkb_inst };
        VkPhysicalDeviceFeatures required_features = {};
        required_features.multiDrawIndirect = VK_TRUE;

        if (m_settings.tesselation)
        {
            required_features.tessellationShader = VK_TRUE;
            required_features.fillModeNonSolid = VK_TRUE;
        }
        auto phys_ret = selector.set_surface(m_surface)
            .set_minimum_version(1, 2)
            .set_required_features(required_features)
            .select();
        if (!phys_ret)
        {
            YERROR("Failed to select Vulkan Physical Device. Error: %s", phys_ret.error().message().c_str());
            return;
        }
        m_physical_device = phys_ret.value().physical_device;
        YINFO("Physical Device Selected: %s", phys_ret.value().name.c_str());

        vkb::DeviceBuilder device_builder{ phys_ret.value() };

        // automatically propagate needed data from instance & physical device
        auto dev_ret = device_builder.build();
        if (!dev_ret)
        {
            YINFO("Failed to create Vulkan device. Error: %s", dev_ret.error().message().c_str());
            return;
        }
        vkb::Device vkb_device = dev_ret.value();
        m_device = vkb_device.device;

        // Queues
        auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
        if (!graphics_queue_ret)
        {
            YINFO("Failed to get graphics queue. Error: %s", graphics_queue_ret.error().message().c_str());
            return;
        }
        m_queues.graphics.queue = graphics_queue_ret.value();
        m_queues.graphics.index = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        auto transfer_queue_ret = vkb_device.get_queue(vkb::QueueType::transfer);
        if (!transfer_queue_ret)
        {
            YINFO("No dedicated transfer queue.");
            m_queues.transfer.queue = m_queues.graphics.queue;
        }
        else
        {
            m_queues.transfer.queue = transfer_queue_ret.value();
            m_queues.transfer.index = vkb_device.get_queue_index(vkb::QueueType::transfer).value();
        }

        auto compute_queue_ret = vkb_device.get_queue(vkb::QueueType::compute);
        if (!compute_queue_ret)
        {
            YINFO("No dedicated compute queue.");
            m_queues.compute.queue = m_queues.graphics.queue;
        }
        else
        {
            m_queues.compute.queue = compute_queue_ret.value();
            m_queues.compute.index = vkb_device.get_queue_index(vkb::QueueType::compute).value();
        }

        m_deletion_queue.Push([&]()
            {
                vkDestroyDevice(m_device, nullptr);
                vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

                vkDestroyInstance(m_instance, nullptr); });
    }

    void VulkanRenderer::InitSwapchain()
    {
        vkb::SwapchainBuilder builder{ m_physical_device, m_device, m_surface };

        VkSurfaceFormatKHR surface_format = {};
        surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
        vkb::Swapchain vkb_swapchain = builder.use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(720, 480)
            .set_desired_format(surface_format)
            .build()
            .value();

        m_swapchain = vkb_swapchain.swapchain;
        m_swapchain_image_format = vkb_swapchain.image_format;
        m_swapchain_images = vkb_swapchain.get_images().value();

        // Swap chain image views are created
        m_swapchain_image_views = vkb_swapchain.get_image_views().value();

        m_deletion_queue.Push([=]()
            {
                for (auto image_view : m_swapchain_image_views)
                {
                    vkDestroyImageView(m_device, image_view, nullptr);
                }

                vkDestroySwapchainKHR(m_device, m_swapchain, nullptr); });
    }

    void VulkanRenderer::InitCommands()
    {
        VkCommandPoolCreateInfo graphics_cp_info = vkinit::CommandPoolCreateInfo(m_queues.graphics.index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        VkCommandPoolCreateInfo compute_cp_info = vkinit::CommandPoolCreateInfo(m_queues.compute.index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        // Create a command pool & buffer for each frame to send render commands async
        for (VulkanFrameContext& ctx : m_frame_context)
        {
            vkCreateCommandPool(m_device, &graphics_cp_info, nullptr, &ctx.command_pool);
            VkCommandBufferAllocateInfo alloc_info = vkinit::CommandBufferAllocInfo(ctx.command_pool);
            vkAllocateCommandBuffers(m_device, &alloc_info, &ctx.command_buffer);

            vkCreateCommandPool(m_device, &compute_cp_info, nullptr, &ctx.compute_command_pool);
            VkCommandBufferAllocateInfo compute_alloc_info = vkinit::CommandBufferAllocInfo(ctx.compute_command_pool);
            vkAllocateCommandBuffers(m_device, &compute_alloc_info, &ctx.compute_command_buffer);

        }

        m_deletion_queue.Push([=]() {
            for (VulkanFrameContext& ctx : m_frame_context)
            {
                vkDestroyCommandPool(m_device, ctx.compute_command_pool, nullptr);
                vkDestroyCommandPool(m_device, ctx.command_pool, nullptr);
            }
            });
    }

    void VulkanRenderer::InitSyncStructures()
    {
        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_info.flags = 0;

        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (auto& ctx : m_frame_context)
        {
            VK_CHECK(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &ctx.present_semaphore));
            VK_CHECK(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &ctx.render_semaphore));

            VK_CHECK(vkCreateFence(m_device, &fence_info, nullptr, &ctx.render_fence));
        }

        m_deletion_queue.Push([&]()
            {
                for (auto& ctx : m_frame_context)
                {
                    vkDestroySemaphore(m_device, ctx.present_semaphore, nullptr);
                    vkDestroySemaphore(m_device, ctx.render_semaphore, nullptr);

                    vkDestroyFence(m_device, ctx.render_fence, nullptr);
                } });
    }

    void VulkanRenderer::InitBlitPipeline()
    {
        std::vector<VulkanDescriptorSet> descriptors;

        // TODO : VulkanShaderModule VulkanResource::CreateShaderModule();
        VkShaderModule vertex_module;
        VkShaderModule fragment_module;
        {
            char* buffer;
            size_t buffer_size;
            if (Platform::FileRead("assets/shaders/lit_shader.vert.spv", &buffer, &buffer_size))
            {
                VkShaderModuleCreateInfo shader_info = {};
                shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                shader_info.pCode = reinterpret_cast<uint32_t*>(buffer);
                shader_info.codeSize = buffer_size;
                VK_CHECK(vkCreateShaderModule(m_device, &shader_info, nullptr, &vertex_module));

                ParseDescriptorSetsFromSpirV(buffer, buffer_size, VK_SHADER_STAGE_VERTEX_BIT, descriptors);
                delete buffer;
            }
            else
            {
                YERROR("Failed to create shader module!");
            }
        }

        {
            char* buffer;
            size_t buffer_size;
            if (Platform::FileRead("assets/shaders/lit_shader.frag.spv", &buffer, &buffer_size))
            {
                VkShaderModuleCreateInfo shader_info = {};
                shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                shader_info.pCode = reinterpret_cast<uint32_t*>(buffer);
                shader_info.codeSize = buffer_size;
                VK_CHECK(vkCreateShaderModule(m_device, &shader_info, nullptr, &fragment_module));

                ParseDescriptorSetsFromSpirV(buffer, buffer_size, VK_SHADER_STAGE_VERTEX_BIT, descriptors);
                delete buffer;
            }
            else
            {
                YERROR("Failed to create shader module!");
            }
        }

        PipelineBuilder builder = {};
        builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertex_module));
        builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_module));

        builder.vertex_input_info = vkinit::PipelineVertexInputStateCreateInfo();
        builder.vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributeDescriptions().size());
        builder.vertex_input_info.pVertexAttributeDescriptions = VertexAttributeDescriptions().data();
        builder.vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexBindingDescriptions().size());
        builder.vertex_input_info.pVertexBindingDescriptions = VertexBindingDescriptions().data();

        builder.input_assembly = vkinit::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        builder.rasterizer = vkinit::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);

        builder.multisampling = vkinit::PipelineMultisampleStateCreateInfo();

        builder.color_blend_attachment = vkinit::PipelineColorBlendAttachmentState();

        builder.depth_stencil = vkinit::PipelineDepthStencilStateCreateInfo(false, false, VK_COMPARE_OP_ALWAYS);

        builder.tesselation_state = {};

        // Build descriptor set layouts
        std::vector<VkDescriptorSetLayout> descriptor_layouts;
        for (VulkanDescriptorSet& descriptor : descriptors)
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
        vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_blit_pipeline_layout);
        builder.pipeline_layout = m_blit_pipeline_layout;

        builder.viewport.x = 0;
        builder.viewport.y = 0;
        builder.viewport.width = 720;
        builder.viewport.height = 480;

        builder.scissor = {};
        builder.scissor.extent.width = 720;
        builder.scissor.extent.height = 480;
        builder.scissor.offset.x = 0;
        builder.scissor.offset.y = 0;

        m_blit_pipeline = builder.Build(m_device, m_swapchain_render_pass);

        // VulkanShaderPass shader_pass = MaterialSystem::CreateShaderPass(some_shader_effect);
        // shader_pass.pipeline 
        // shader_pass.

        m_deletion_queue.Push([=]()
            {
                vkDestroyPipelineLayout(m_device, m_blit_pipeline_layout, nullptr);
                vkDestroyPipeline(m_device, m_blit_pipeline, nullptr);
            });
    }

    void VulkanRenderer::InitSwapchainRenderPass()
    {
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = m_swapchain_image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_reference = {};
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_reference;

        VkAttachmentDescription attachments[] = { color_attachment };
        VkSubpassDescription subpasses[] = { subpass };

        VkRenderPassCreateInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.pNext = nullptr;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = attachments;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = subpasses;

        VK_CHECK(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_swapchain_render_pass));
        m_deletion_queue.Push([=]()
            { vkDestroyRenderPass(m_device, m_swapchain_render_pass, nullptr); });
    }

    void VulkanRenderer::InitSwapchainFramebuffers()
    {
        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = m_swapchain_render_pass;

        framebuffer_info.width = 720;
        framebuffer_info.height = 480;
        framebuffer_info.layers = 1;

        framebuffer_info.attachmentCount = 1;

        m_swapchain_framebuffers.resize(m_swapchain_images.size());
        for (int i = 0; i < m_swapchain_framebuffers.size(); i++)
        {
            framebuffer_info.pAttachments = &m_swapchain_image_views[i];
            VK_CHECK(vkCreateFramebuffer(m_device, &framebuffer_info, nullptr, &m_swapchain_framebuffers[i]));
            m_deletion_queue.Push([&]()
                { vkDestroyFramebuffer(m_device, m_swapchain_framebuffers[i], nullptr); });
        }
    }

    void VulkanRenderer::InitForwardPass()
    {
    }
};