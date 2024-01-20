#include <VkBootstrap.h>

#include "VulkanRenderer.h"

#include "Platform/Platform.h"
#include "Core/Time.h"
#include "Core/Log.h"

#include "VulkanMesh.h"
#include "VulkanTexture.h"
#include "Math/MatrixTransform.h"

namespace yoyo
{
    Ref<Renderer> CreateRenderer()
    {
        return CreateRef<VulkanRenderer>();
    }

    VulkanRenderer::VulkanRenderer()
        : Renderer({ RendererType::Vulkan, 2, true }),
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

        m_material_system = CreateRef<VulkanMaterialSystem>();
        m_material_system->Init(this);

        InitForwardPass();
        InitForwardPassAttachments();
        InitForwardPassFramebufffer();

        InitBlitPipeline();

        // TODO: All uploads must be done immediately uppon creation and update
        m_screen_quad = CreateRef<VulkanMesh>();
        m_screen_quad->vertices =
        {
            {{-1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  1.00}},
            {{ 1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  1.00}},
            {{-1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  0.00}},
            {{ 1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  1.00}},
            {{ 1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  0.00}},
            {{-1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  0.00}},
        };
        m_resource_manager->UploadMesh(m_screen_quad);

        // Init mesh passes
        m_scene.forward_pass = CreateRef<MeshPass>();
        m_scene.transparent_forward_pass = CreateRef<MeshPass>();
        m_scene.shadow_pass = CreateRef<MeshPass>();

        // TODO: Move to Application Layer
        m_cube_mesh = CreateRef<VulkanMesh>();
        m_cube_mesh->vertices = {
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.625,  0.50}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.875,  0.50}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.625,  0.25}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.875,  0.50}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.875,  0.25}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.625,  0.25}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.375,  0.25}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.625,  0.25}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.375,  0.00}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.625,  0.25}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.625,  0.00}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.375,  0.00}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.375,  1.00}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.625,  1.00}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.375,  0.75}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.625,  1.00}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.625,  0.75}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.375,  0.75}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.125,  0.50}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.375,  0.50}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.125,  0.25}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.375,  0.50}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.375,  0.25}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.125,  0.25}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.375,  0.50}},
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.625,  0.50}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.375,  0.25}},
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.625,  0.50}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.625,  0.25}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.375,  0.25}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.375,  0.75}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.625,  0.75}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.375,  0.50}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.625,  0.75}},
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.625,  0.50}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.375,  0.50}},
        };
        m_resource_manager->UploadMesh(m_cube_mesh);

        // Describe effect
        // TODO: Cache Effect description
        Ref<VulkanShaderEffect> lit_effect = CreateRef<VulkanShaderEffect>();
        Ref<VulkanShaderModule> vertex_module = m_resource_manager->CreateShaderModule("assets/shaders/lit_shader.vert.spv");
        lit_effect->PushShader(vertex_module, VK_SHADER_STAGE_VERTEX_BIT);

        Ref<VulkanShaderModule> fragment_module = m_resource_manager->CreateShaderModule("assets/shaders/lit_shader.frag.spv");
        lit_effect->PushShader(fragment_module, VK_SHADER_STAGE_FRAGMENT_BIT);

        // Create or get shader pass from effect description
        lit_shader_pass = m_material_system->CreateShaderPass(m_forward_pass, lit_effect);

        // Create shader (Effect Template)
        Ref<Shader> lit_shader = CreateRef<Shader>();
        lit_shader->shader_passes[MeshPassType::Forward] = lit_shader_pass;

        // Scene Creation TODO: Make scene incremental build
        {
            Ref<Texture> container_diffuse = Texture::LoadFromAsset("assets/textures/container2.yo");
            m_resource_manager->UploadTexture(std::static_pointer_cast<VulkanTexture>(container_diffuse));

            Ref<Material> red_material = m_material_system->CreateMaterial(std::static_pointer_cast<VulkanShader>(lit_shader));
            red_material->SetTexture(MaterialTextureType::MainTexture, container_diffuse);
            red_material->color = Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            red_material->SetVec4("diffuse_color", Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            red_material->SetVec4("specular_color", Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

            Ref<MeshPassObject> pass_obj = CreateRef<MeshPassObject>();
            pass_obj->material = red_material;
            pass_obj->mesh = m_cube_mesh;
            pass_obj->model_matrix = TranslationMat4x4({ 0.0, 0.0, -5.0f });

            m_scene.forward_pass->renderables.push_back(pass_obj);
        }
    }

    void VulkanRenderer::Shutdown()
    {
        m_descriptor_allocator->CleanUp();
        m_descriptor_layout_cache->Clear();

        m_material_system->Shutdown();
        m_resource_manager->Shutdown();

        m_deletion_queue.Flush();
    }

    bool VulkanRenderer::BeginFrame(const RenderPacket& rp)
    {
        vkWaitForFences(m_device, 1, &m_frame_context[m_frame_count].render_fence, VK_TRUE, 1000000000);
        vkResetFences(m_device, 1, &m_frame_context[m_frame_count].render_fence);

        // Update Scene data
        void* data;
        m_resource_manager->MapMemory(scene_data_uniform_buffer.allocation, &data);
        SceneData scene_data = {};
        static float angle = 0;
        scene_data.view = RotateMat4x4(angle * (1.0f / 144.0f), {}) * TranslationMat4x4({ 2.0f * sin(angle * (1.0f / 144.0f)), 0.0f, -5.0f });
        angle += 20.0f * (Y_PI / 180.0f);
        scene_data.proj = PerspectiveProjectionMat4x4((90.0f * Y_PI) / 180.0f, 720.0f / 480.0f, 0.1f, 100.0f);
        memcpy(data, &scene_data, sizeof(SceneData));
        m_resource_manager->UnmapMemory(scene_data_uniform_buffer.allocation);

        m_swapchain_index = -1;
        VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_frame_context[m_frame_count].present_semaphore, VK_NULL_HANDLE, &m_swapchain_index));

        VkCommandBufferBeginInfo cmd_begin_info = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        VkCommandBuffer cmd = m_frame_context[m_frame_count].command_buffer;
        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

        // TODO: Bind shadow pass and draw shadow mesh passes

        // TODO: Bind main render pass and draw mesh passes
        VkRect2D rect = {};
        rect.offset.x = 0;
        rect.offset.y = 0;
        rect.extent.width = 720;
        rect.extent.height = 480;

        VkClearValue color_clear = {};
        color_clear.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        VkClearValue depth_clear = {};
        depth_clear.depthStencil = { 1.0f, 0 };

        VkClearValue clear_values[2] = { color_clear, depth_clear };
        VkRenderPassBeginInfo forward_pass_begin = vkinit::RenderPassBeginInfo(m_forward_frame_buffer, m_forward_pass, rect, clear_values, 2);
        vkCmdBeginRenderPass(cmd, &forward_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

        // Move to RendererLayer
        {
            VulkanRenderContext ctx = {};
            ctx.cmd = cmd;

            // ... draw forward for each mesh passes

            // TODO: Build renderable batches and draw indirect
            // TODO: sort by material forward Shader Pass
            for (Ref<MeshPassObject> obj : m_scene.forward_pass->renderables)
            {
                // Bind material forward pipeline
                auto shader_pass = obj->material->shader->shader_passes[MeshPassType::Forward];
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->pipeline);

                uint32_t dynamic_offset = 0;
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->layout, 0, 1, &m_forward_pass_ds, 0, &dynamic_offset);

                // FOR EACH GAMEOBJECT

                // TODO: Make sure before all draws of material
                static float angle = 0.0f;
                obj->material->SetVec4("diffuse_color", obj->material->color * sin(angle));
                angle += (Y_PI * 25.0f / 180.0f) * Time::DeltaTime();
                if (obj->material->Dirty())
                {
                    m_material_system->UpdateMaterial(std::static_pointer_cast<VulkanMaterial>(obj->material));
                }

                // Bind material descriptors 
                obj->material->Bind(&ctx, MeshPassType::Forward);

                // Bind mesh
                obj->mesh->Bind(&ctx);

                // Draw every object with material
                if (!obj->mesh->indices.empty())
                {
                    vkCmdDrawIndexed(cmd, obj->mesh->indices.size(), 1, 0, 0, 0);
                }
                else
                {
                    vkCmdDraw(cmd, obj->mesh->vertices.size(), 1, 0, 0);
                }
            }

            vkCmdEndRenderPass(cmd);
        }

        // TODO: Bind post processing render pass and draw post process passes
        return true;
    }

    void VulkanRenderer::EndFrame()
    {
        // Blit offscreen target to swapchain
        VkCommandBuffer cmd = m_frame_context[m_frame_count].command_buffer;

        // Blit output_texture barrier
        VkImageMemoryBarrier blit_output_texture_barrier = {};
        blit_output_texture_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        blit_output_texture_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        blit_output_texture_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        blit_output_texture_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        blit_output_texture_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        blit_output_texture_barrier.image = m_forward_pass_color_texture.image;

        VkImageSubresourceRange blit_texture_range = {};
        blit_texture_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_texture_range.layerCount = 1;
        blit_texture_range.baseArrayLayer = 0;
        blit_texture_range.levelCount = 1;

        blit_output_texture_barrier.subresourceRange = blit_texture_range;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, 0, 1, &blit_output_texture_barrier);

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

        uint32_t dynamic_offset = 0;
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blit_pipeline_layout, 0, 1, &m_blit_output_texture_ds, 0, &dynamic_offset);

        VulkanRenderContext ctx = {};
        ctx.cmd = cmd;

        m_screen_quad->Bind(&ctx);
        if (!m_screen_quad->indices.empty())
        {
            vkCmdDrawIndexed(cmd, m_screen_quad->indices.size(), 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(cmd, m_screen_quad->vertices.size(), 1, 0, 0);
        }

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
        m_physical_device_properties = phys_ret.value().properties;
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
        surface_format.format = m_forward_pass_format;
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
        std::vector<VulkanDescriptorSetInformation> descriptors_info;

        Ref<VulkanShaderModule> vertex_module = m_resource_manager->CreateShaderModule("assets/shaders/full_screen_shader.vert.spv");
        ParseDescriptorSetsFromSpirV(vertex_module->code.data(), vertex_module->code.size() * sizeof(uint32_t), VK_SHADER_STAGE_VERTEX_BIT, descriptors_info);

        Ref<VulkanShaderModule> fragment_module = m_resource_manager->CreateShaderModule("assets/shaders/blit_shader.frag.spv");
        ParseDescriptorSetsFromSpirV(fragment_module->code.data(), fragment_module->code.size() * sizeof(uint32_t), VK_SHADER_STAGE_FRAGMENT_BIT, descriptors_info);

        PipelineBuilder builder = {};
        builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertex_module->module));
        builder.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_module->module));

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
        for (VulkanDescriptorSetInformation& descriptor : descriptors_info)
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

        m_deletion_queue.Push([=]()
            {
                vkDestroyPipelineLayout(m_device, m_blit_pipeline_layout, nullptr);
                vkDestroyPipeline(m_device, m_blit_pipeline, nullptr);
            });

        // TODO: move to resource manager
        VkSampler linear_sampler = {};
        VkSamplerCreateInfo linear_sampler_info = vkinit::SamplerCreateInfo(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        vkCreateSampler(m_device, &linear_sampler_info, nullptr, &linear_sampler);

        // Descriptors
        VkDescriptorImageInfo output_image_info = {};
        output_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        output_image_info.imageView = m_forward_pass_color_texture_view;
        output_image_info.sampler = linear_sampler;

        DescriptorBuilder ds_builder = {};
        ds_builder.Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
            .BindImage(0, &output_image_info, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build(&m_blit_output_texture_ds, &m_blit_pass_ds_layout);
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
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depth_attachment = {};
        depth_attachment.format = VK_FORMAT_D32_SFLOAT;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref = {};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };
        VkRenderPassCreateInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

        render_pass_info.attachmentCount = 2;
        render_pass_info.pAttachments = attachments;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        VK_CHECK(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_forward_pass));

        m_deletion_queue.Push([=]()
            {
                vkDestroyRenderPass(m_device, m_forward_pass, nullptr);
            });

        // Descriptors
        size_t padded_scene_data_size = m_resource_manager->PadToUniformBufferSize(sizeof(SceneData));
        scene_data_uniform_buffer = m_resource_manager->CreateBuffer<SceneData>(padded_scene_data_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        VkDescriptorBufferInfo scene_data_info = {};
        scene_data_info.buffer = scene_data_uniform_buffer.buffer;
        scene_data_info.offset = 0;
        scene_data_info.range = sizeof(SceneData);

        DescriptorBuilder ds_builder = {};
        ds_builder.Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
            .BindBuffer(0, &scene_data_info, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build(&m_forward_pass_ds, &m_forward_pass_ds_layout);
    }

    void VulkanRenderer::InitForwardPassFramebufffer()
    {
        VkImageView views[2] = { m_forward_pass_color_texture_view, m_forward_pass_depth_texture_view };

        VkFramebufferCreateInfo frame_buffer_info = {};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.attachmentCount = 2;
        frame_buffer_info.pAttachments = views;
        frame_buffer_info.renderPass = m_forward_pass;
        frame_buffer_info.width = 720;
        frame_buffer_info.height = 480;
        frame_buffer_info.layers = 1;

        VK_CHECK(vkCreateFramebuffer(m_device, &frame_buffer_info, nullptr, &m_forward_frame_buffer));

        m_deletion_queue.Push([=]() {
            vkDestroyFramebuffer(m_device, m_forward_frame_buffer, nullptr);
            });
    }

    void VulkanRenderer::InitForwardPassAttachments()
    {
        VkExtent3D extent = {};
        extent.width = 720;
        extent.height = 480;
        extent.depth = 1;

        m_forward_pass_color_texture = m_resource_manager->CreateImage(extent, m_forward_pass_format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        m_forward_pass_depth_texture = m_resource_manager->CreateImage(extent, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        VkImageViewCreateInfo color_info = vkinit::ImageViewCreateInfo(m_forward_pass_color_texture.image, m_forward_pass_format, VK_IMAGE_ASPECT_COLOR_BIT);
        VkImageViewCreateInfo depth_info = vkinit::ImageViewCreateInfo(m_forward_pass_depth_texture.image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

        VK_CHECK(vkCreateImageView(m_device, &color_info, nullptr, &m_forward_pass_color_texture_view));
        VK_CHECK(vkCreateImageView(m_device, &depth_info, nullptr, &m_forward_pass_depth_texture_view));

        m_deletion_queue.Push([=]() {
            vkDestroyImageView(m_device, m_forward_pass_color_texture_view, nullptr);
            vkDestroyImageView(m_device, m_forward_pass_depth_texture_view, nullptr);
            });
    }
};