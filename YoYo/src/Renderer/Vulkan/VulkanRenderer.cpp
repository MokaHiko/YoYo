#include <VkBootstrap.h>

#include "VulkanRenderer.h"

#include "Platform/Platform.h"
#include "Core/Time.h"
#include "Core/Log.h"

#include "VulkanMesh.h"
#include "VulkanTexture.h"

#include "Renderer/Camera.h"
#include "Renderer/Light.h"

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

    void* VulkanRenderer::RenderContext()
    {
        return &m_render_context;
    }

    void VulkanRenderer::Init()
    {
        m_frame_count = 0;
        m_frame_context.resize(Settings().max_frames_in_flight);

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

        VulkanResourceManager::Init(this);

        m_material_system = CreateRef<VulkanMaterialSystem>();
        m_material_system->Init(this);

        InitSceneResources();

        InitShadowPass();
        InitShadowPassAttachments();
        InitShadowPassFramebufffer();

        InitForwardPass();
        InitForwardPassAttachments();
        InitForwardPassFramebufffer();

        InitBlitPipeline();

        // TODO: All uploads must be done immediately uppon creation and update
        m_screen_quad = CreateRef<VulkanMesh>();
        m_screen_quad->vertices =
        {
            {{-1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  0.00}},
            {{ 1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  0.00}},
            {{-1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  1.00}},
            {{ 1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  0.00}},
            {{ 1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  1.00}},
            {{-1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  1.00}},
        };
        m_screen_quad->UploadMeshData();

        // Describe effect
        Ref<VulkanShaderEffect> lit_effect = CreateRef<VulkanShaderEffect>();
        {
            Ref<VulkanShaderModule> vertex_module = VulkanResourceManager::CreateShaderModule("assets/shaders/lit_shader.vert.spv");
            lit_effect->PushShader(vertex_module, VK_SHADER_STAGE_VERTEX_BIT);

            Ref<VulkanShaderModule> fragment_module = VulkanResourceManager::CreateShaderModule("assets/shaders/lit_shader.frag.spv");
            lit_effect->PushShader(fragment_module, VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        auto lit_shader_pass = m_material_system->CreateShaderPass(m_forward_pass, lit_effect);

        Ref<VulkanShaderEffect> lit_instanced_effect = CreateRef<VulkanShaderEffect>();
        {
            Ref<VulkanShaderModule> vertex_module = VulkanResourceManager::CreateShaderModule("assets/shaders/lit_instanced_shader.vert.spv");
            lit_instanced_effect->PushShader(vertex_module, VK_SHADER_STAGE_VERTEX_BIT);

            Ref<VulkanShaderModule> fragment_module = VulkanResourceManager::CreateShaderModule("assets/shaders/lit_shader.frag.spv");
            lit_instanced_effect->PushShader(fragment_module, VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        auto lit_instanced_shader_pass = m_material_system->CreateShaderPass(m_forward_pass, lit_instanced_effect);

        // Create or get shader pass from effect description
        Ref<VulkanShaderEffect> shadow_lit_effect = CreateRef<VulkanShaderEffect>();
        {
            Ref<VulkanShaderModule> vertex_module = VulkanResourceManager::CreateShaderModule("assets/shaders/offscreen_shadow_shader.vert.spv");
            shadow_lit_effect->PushShader(vertex_module, VK_SHADER_STAGE_VERTEX_BIT);

            Ref<VulkanShaderModule> fragment_module = VulkanResourceManager::CreateShaderModule("assets/shaders/offscreen_shadow_shader.frag.spv");
            shadow_lit_effect->PushShader(fragment_module, VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        auto shadow_pass = m_material_system->CreateShaderPass(m_shadow_render_pass, shadow_lit_effect);

        // Create or get shader pass from effect description
        Ref<VulkanShaderEffect> shadow_lit_instanced_effect = CreateRef<VulkanShaderEffect>();
        {
            Ref<VulkanShaderModule> vertex_module = VulkanResourceManager::CreateShaderModule("assets/shaders/offscreen_shadow_instanced_shader.vert.spv");
            shadow_lit_instanced_effect->PushShader(vertex_module, VK_SHADER_STAGE_VERTEX_BIT);

            Ref<VulkanShaderModule> fragment_module = VulkanResourceManager::CreateShaderModule("assets/shaders/offscreen_shadow_shader.frag.spv");
            shadow_lit_instanced_effect->PushShader(fragment_module, VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        auto shadow_instanced_pass = m_material_system->CreateShaderPass(m_shadow_render_pass, shadow_lit_instanced_effect);

        // Create shader (Effect Template)
        Ref<Shader> lit_shader = Shader::Create("lit_shader");
        lit_shader->shader_passes[MeshPassType::Forward] = lit_shader_pass;
        lit_shader->shader_passes[MeshPassType::Shadow] = shadow_pass;

        Ref<Shader> lit_instanced_shader = Shader::Create("lit_instanced_shader", true);
        lit_instanced_shader->shader_passes[MeshPassType::Forward] = lit_instanced_shader_pass;
        lit_instanced_shader->shader_passes[MeshPassType::Shadow] = shadow_instanced_pass;
    }

    void VulkanRenderer::Shutdown()
    {
        vkDeviceWaitIdle(Device());

        m_deletion_queue.Flush();

        m_descriptor_allocator->CleanUp();
        m_descriptor_layout_cache->Clear();

        m_material_system->Shutdown();

        VulkanResourceManager::Shutdown();

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        //vkDestroyDebugUtilsMessengerEXT(m_de)
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    bool VulkanRenderer::BeginFrame(const Ref<RenderScene> scene)
    {
        vkWaitForFences(m_device, 1, &m_frame_context[m_frame_count].render_fence, VK_TRUE, 1000000000);
        vkResetFences(m_device, 1, &m_frame_context[m_frame_count].render_fence);

        m_swapchain_index = -1;
        VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_frame_context[m_frame_count].present_semaphore, VK_NULL_HANDLE, &m_swapchain_index));

        // Set up profiling
        Profile().draw_calls = 0;

        // Update Scene data
        {
            void* data;
            VulkanResourceManager::MapMemory(m_scene_data_uniform_buffer.allocation, &data);

            SceneData scene_data = {};

            if (!scene->camera)
            {
                scene_data.view = Mat4x4(0.0f);
                scene_data.proj = Mat4x4(0.0f);
            }
            else
            {
                scene_data.view = scene->camera->View();
                scene_data.proj = scene->camera->Projection();
            }

            scene_data.point_light_count = static_cast<uint32_t>(scene->point_lights.size());
            scene_data.dir_light_count = static_cast<uint32_t>(scene->directional_lights.size());
            scene_data.spot_light_count = 0;
            scene_data.area_light_count = 0;

            memcpy(data, &scene_data, sizeof(SceneData));
            VulkanResourceManager::UnmapMemory(m_scene_data_uniform_buffer.allocation);
        }

        {
            char* data;
            VulkanResourceManager::MapMemory(m_directional_lights_buffer.allocation, (void**)&data);
            const size_t padded_directional_light_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(DirectionalLight));

            for (int i = 0; i < scene->directional_lights.size(); i++)
            {
                memcpy(data + (i * padded_directional_light_size), scene->directional_lights[i].get(), sizeof(DirectionalLight));
            }

            VulkanResourceManager::UnmapMemory(m_directional_lights_buffer.allocation);
        }

        // Update ObjectData
        {
            // The mesh pass objects ID is used for the index into buffer
            char* data;
            VulkanResourceManager::MapMemory(m_object_data_buffers[m_frame_count].allocation, (void**)&data);
            const size_t padded_object_data_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(ObjectData));

            for (const Ref<MeshPassObject>& renderable : scene->forward_pass->renderables)
            {
                ObjectData obj_data = {};
                obj_data.model_matrix = renderable->model_matrix;

                size_t offset = static_cast<size_t>(renderable->Id()) * padded_object_data_size;
                memcpy(data + offset, &obj_data, sizeof(ObjectData));
            }

            VulkanResourceManager::UnmapMemory(m_object_data_buffers[m_frame_count].allocation);
        }

        // Update InstancedData
        {
            // TODO: Dirty Check
            char* data;
            VulkanResourceManager::MapMemory(m_instanced_data_buffers[m_frame_count].allocation, (void**)&data);
            const size_t padded_instanced_data_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(InstancedData));

            int ctr = 0;
            for (const Ref<RenderableBatch>& batch : scene->forward_flat_batches)
            {
                if (!batch->material->instanced)
                {
                    continue;
                }

                batch->instance_index = ctr;
                for (const Ref<MeshPassObject>& renderable : batch->renderables)
                {
                    InstancedData instanced_data = {};
                    instanced_data.id = renderable->Id();
                    memcpy(data + (ctr++ * padded_instanced_data_size), &instanced_data, sizeof(InstancedData));
                }
            };

            VulkanResourceManager::UnmapMemory(m_instanced_data_buffers[m_frame_count].allocation);
        }

        VkCommandBufferBeginInfo cmd_begin_info = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        VkCommandBuffer cmd = m_frame_context[m_frame_count].command_buffer;

        // Update render context
        m_render_context.cmd = cmd;

        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

        // Bind shadow pass and draw shadow mesh passes
        {
            VkRect2D rect = {};
            rect.offset.x = 0;
            rect.offset.y = 0;
            rect.extent.width = Settings().width;
            rect.extent.height = Settings().height;

            VkClearValue depth_clear = {};
            depth_clear.depthStencil = { 1.0f, 0 };

            VkClearValue clear_values[1] = { depth_clear };

            VkRenderPassBeginInfo shadow_pass_begin = vkinit::RenderPassBeginInfo(m_shadow_frame_buffer, m_shadow_render_pass, rect, clear_values, 1);
            vkCmdBeginRenderPass(cmd, &shadow_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

            for (const Ref<RenderableBatch>& batch : scene->forward_flat_batches)
            {
                // Bind material forward pipeline
                auto shader_pass = batch->material->shader->shader_passes[MeshPassType::Shadow];
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->pipeline);

                uint32_t dynamic_offset = 0;
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->layout, 0, 1, &m_shadow_pass_dsets[m_frame_count], 0, &dynamic_offset);

                // Bind mesh
                batch->mesh->Bind(&m_render_context);

                if (!batch->material->instanced)
                {
                    for (Ref<MeshPassObject> obj : batch->renderables)
                    {
                        // Draw every object w mesh and material
                        if (!batch->mesh->indices.empty())
                        {
                            Profile().draw_calls++;
                            vkCmdDrawIndexed(cmd, obj->mesh->indices.size(), 1, 0, 0, obj->Id());
                        }
                        else
                        {
                            Profile().draw_calls++;
                            vkCmdDraw(cmd, obj->mesh->vertices.size(), 1, 0, obj->Id());
                        }
                    }
                }
                else
                {
                    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->layout, SHADOW_PASS_INSTANCED_OBJECT_DATA_SET_INDEX, 1, &m_instanced_dsets[m_frame_count], 0, &dynamic_offset);

                    // Draw every object w mesh and material
                    if (!batch->mesh->indices.empty())
                    {
                        Profile().draw_calls++;
                        vkCmdDrawIndexed(cmd, batch->mesh->indices.size(), batch->renderables.size(), 0, 0, batch->instance_index);
                    }
                    else
                    {
                        Profile().draw_calls++;
                        vkCmdDraw(cmd, batch->mesh->vertices.size(), batch->renderables.size(), 0, batch->instance_index);
                    }
                }
            }

            vkCmdEndRenderPass(cmd);
        }

        // Blit output_texture barrier
        VkImageMemoryBarrier shadow_map_barrier = {};
        shadow_map_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        shadow_map_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        shadow_map_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        shadow_map_barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        shadow_map_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        shadow_map_barrier.image = m_shadow_pass_depth_texture.image;

        VkImageSubresourceRange shadow_map_range = {};
        shadow_map_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        shadow_map_range.layerCount = 1;
        shadow_map_range.baseArrayLayer = 0;
        shadow_map_range.levelCount = 1;

        shadow_map_barrier.subresourceRange = shadow_map_range;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, 0, 1, &shadow_map_barrier);

        // Bind forward render pass and draw mesh passes
        {
            VkRect2D rect = {};
            rect.offset.x = 0;
            rect.offset.y = 0;
            rect.extent.width = Settings().width;
            rect.extent.height = Settings().height;

            VkClearValue color_clear = {};
            color_clear.color = { 1.0f, 1.0f, 1.0f, 1.0f };
            VkClearValue depth_clear = {};
            depth_clear.depthStencil = { 1.0f, 0 };

            VkClearValue clear_values[2] = { color_clear, depth_clear };
            VkRenderPassBeginInfo forward_pass_begin = vkinit::RenderPassBeginInfo(m_forward_frame_buffer, m_forward_pass, rect, clear_values, 2);
            vkCmdBeginRenderPass(cmd, &forward_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

            // Draw each forward pass by batch
            for (const Ref<RenderableBatch>& batch : scene->forward_flat_batches)
            {
                // Bind material forward pipeline
                const auto shader_pass = batch->material->shader->shader_passes[MeshPassType::Forward];
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->pipeline);

                const uint32_t dynamic_offset = 0;
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->layout, 0, 1, &m_forward_pass_dsets[m_frame_count], 0, &dynamic_offset);

                if (batch->material->Dirty()) { m_material_system->UpdateMaterial(std::static_pointer_cast<VulkanMaterial>(batch->material)); }
                batch->material->Bind(&m_render_context, MeshPassType::Forward);
                batch->mesh->Bind(&m_render_context);

                if (!batch->material->instanced)
                {
                    for (const auto& obj : batch->renderables)
                    {
                        if (!obj->mesh->indices.empty())
                        {
                            Profile().draw_calls++;
                            vkCmdDrawIndexed(cmd, obj->mesh->indices.size(), 1, 0, 0, obj->Id());
                        }
                        else
                        {
                            Profile().draw_calls++;
                            vkCmdDraw(cmd, obj->mesh->vertices.size(), 1, 0, obj->Id());
                        }
                    }
                }
                else
                {
                    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pass->layout, INSTANCED_OBJECT_DATA_SET_INDEX, 1, &m_instanced_dsets[m_frame_count], 0, &dynamic_offset);

                    if (!batch->mesh->indices.empty())
                    {
                        Profile().draw_calls++;
                        vkCmdDrawIndexed(cmd, batch->mesh->indices.size(), batch->renderables.size(), 0, 0, batch->instance_index);
                    }
                    else
                    {
                        Profile().draw_calls++;
                        vkCmdDraw(cmd, batch->mesh->vertices.size(), batch->renderables.size(), 0, batch->instance_index);
                    }
                }
            }
            vkCmdEndRenderPass(cmd);
        }

        // TODO: Bind post processing render pass and draw post process passes
        // For(pass : post_process_passes) {}

        return true;
    }

    void VulkanRenderer::BeginBlitPass()
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
        rect.extent.width = Settings().width;
        rect.extent.height = Settings().height;

        VkClearValue clear_value = {};
        clear_value.color = { 1.0f, 1.0f, 1.0f, 1.0f };

        VkRenderPassBeginInfo renderpass_begin_info = vkinit::RenderPassBeginInfo(m_swapchain_framebuffers[m_swapchain_index], m_swapchain_render_pass, rect, &clear_value, 1);
        vkCmdBeginRenderPass(cmd, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blit_pipeline);

        uint32_t dynamic_offset = 0;
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blit_pipeline_layout, 0, 1, &m_blit_output_texture_ds, 0, &dynamic_offset);

        m_screen_quad->Bind(&m_render_context);
        if (!m_screen_quad->indices.empty())
        {
            Profile().draw_calls++;
            vkCmdDrawIndexed(cmd, m_screen_quad->indices.size(), 1, 0, 0, 0);
        }
        else
        {
            Profile().draw_calls++;
            vkCmdDraw(cmd, m_screen_quad->vertices.size(), 1, 0, 0);
        }
    }

    void VulkanRenderer::EndBlitPass()
    {
        // Blit offscreen target to swapchain
        VkCommandBuffer cmd = m_frame_context[m_frame_count].command_buffer;

        vkCmdEndRenderPass(cmd);
    }

    void VulkanRenderer::EndFrame()
    {

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

        m_frame_count = (m_frame_count + 1) % Settings().max_frames_in_flight;
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
        m_debug_messenger = vkb_inst.debug_messenger;
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

        if (Settings().tesselation)
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

        VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
        shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        shader_draw_parameters_features.pNext = nullptr;
        shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

        device_builder.add_pNext(&shader_draw_parameters_features);
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
    }

    void VulkanRenderer::InitSwapchain()
    {
        vkb::SwapchainBuilder builder{ m_physical_device, m_device, m_surface };

        VkSurfaceFormatKHR surface_format = {};
        surface_format.format = m_forward_pass_format;
        vkb::Swapchain vkb_swapchain = builder.use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(Settings().width, Settings().height)
            .set_desired_format(surface_format)
            .build()
            .value();

        m_swapchain = vkb_swapchain.swapchain;
        m_swapchain_image_format = vkb_swapchain.image_format;
        m_swapchain_images = vkb_swapchain.get_images().value();

        // Swap chain image views are created
        m_swapchain_image_views = vkb_swapchain.get_image_views().value();

        m_deletion_queue.Push([=]() {
            for (auto image_view : m_swapchain_image_views)
            {
                vkDestroyImageView(m_device, image_view, nullptr);
            }

            vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            });
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

        Ref<VulkanShaderModule> vertex_module = VulkanResourceManager::CreateShaderModule("assets/shaders/full_screen_shader.vert.spv");
        ParseDescriptorSetsFromSpirV(vertex_module->code.data(), vertex_module->code.size() * sizeof(uint32_t), VK_SHADER_STAGE_VERTEX_BIT, descriptors_info);

        Ref<VulkanShaderModule> fragment_module = VulkanResourceManager::CreateShaderModule("assets/shaders/blit_shader.frag.spv");
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
        builder.viewport.width = Settings().width;
        builder.viewport.height = Settings().height;

        builder.scissor = {};
        builder.scissor.extent.width = Settings().width;
        builder.scissor.extent.height = Settings().height;
        builder.scissor.offset.x = 0;
        builder.scissor.offset.y = 0;

        m_blit_pipeline = builder.Build(m_device, m_swapchain_render_pass);

        m_deletion_queue.Push([=]()
            {
                vkDestroyPipelineLayout(m_device, m_blit_pipeline_layout, nullptr);
                vkDestroyPipeline(m_device, m_blit_pipeline, nullptr);
            });

        // Descriptors
        VkDescriptorImageInfo output_image_info = {};
        output_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        output_image_info.imageView = m_forward_pass_color_texture_view;
        output_image_info.sampler = m_material_system->LinearSampler();

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

        framebuffer_info.width = Settings().width;
        framebuffer_info.height = Settings().height;
        framebuffer_info.layers = 1;

        framebuffer_info.attachmentCount = 1;

        m_swapchain_framebuffers.resize(m_swapchain_images.size());
        for (int i = 0; i < m_swapchain_framebuffers.size(); i++)
        {
            framebuffer_info.pAttachments = &m_swapchain_image_views[i];
            VK_CHECK(vkCreateFramebuffer(m_device, &framebuffer_info, nullptr, &m_swapchain_framebuffers[i]));
            m_deletion_queue.Push([=]()
                { vkDestroyFramebuffer(m_device, m_swapchain_framebuffers[i], nullptr); });
        }
    }

    void VulkanRenderer::InitShadowPass()
    {
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
        depth_attachment_ref.attachment = 0;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 0;
        subpass.pColorAttachments = nullptr;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkAttachmentDescription attachments[] = { depth_attachment };
        VkRenderPassCreateInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = attachments;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        VK_CHECK(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_shadow_render_pass));

        m_deletion_queue.Push([=]() {
            vkDestroyRenderPass(m_device, m_shadow_render_pass, nullptr);
            });

        // Scene resources descriptors
        m_shadow_pass_dsets.resize(Settings().max_frames_in_flight);
        for (int i = 0; i < Settings().max_frames_in_flight; i++)
        {
            VkDescriptorBufferInfo scene_data_info = {};
            scene_data_info.buffer = m_scene_data_uniform_buffer.buffer;
            scene_data_info.offset = 0;
            scene_data_info.range = sizeof(SceneData);

            VkDescriptorBufferInfo dir_light_data_info = {};
            dir_light_data_info.buffer = m_directional_lights_buffer.buffer;
            dir_light_data_info.offset = 0;
            dir_light_data_info.range = VK_WHOLE_SIZE;

            VkDescriptorBufferInfo obj_data_info = {};
            obj_data_info.buffer = m_object_data_buffers[i].buffer;
            obj_data_info.offset = 0;
            obj_data_info.range = VK_WHOLE_SIZE;

            DescriptorBuilder ds_builder = {};
            ds_builder.Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
                .BindBuffer(SCENE_DATA_DESCRIPTOR_SET_BINDING, &scene_data_info, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .BindBuffer(DIRECTIONAL_LIGHTS_SET_BINDING, &dir_light_data_info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .BindBuffer(OBJECT_DATA_SET_BINDING, &obj_data_info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .Build(&m_shadow_pass_dsets[i], &m_shadow_pass_ds_layout);
        }
    }

    void VulkanRenderer::InitShadowPassAttachments()
    {
        VkExtent3D extent = {};
        extent.width = Settings().width;
        extent.height = Settings().height;
        extent.depth = 1;

        m_shadow_pass_depth_texture = VulkanResourceManager::CreateImage(extent, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        VkImageViewCreateInfo depth_info = vkinit::ImageViewCreateInfo(m_shadow_pass_depth_texture.image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

        VK_CHECK(vkCreateImageView(m_device, &depth_info, nullptr, &m_shadow_pass_depth_texture_view));

        m_deletion_queue.Push([=]() {
            vkDestroyImageView(m_device, m_shadow_pass_depth_texture_view, nullptr);
            });
    }

    void VulkanRenderer::InitShadowPassFramebufffer()
    {
        VkImageView views[1] = { m_shadow_pass_depth_texture_view };

        VkFramebufferCreateInfo frame_buffer_info = {};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.attachmentCount = 1;
        frame_buffer_info.pAttachments = views;
        frame_buffer_info.renderPass = m_shadow_render_pass;
        frame_buffer_info.width = Settings().width;
        frame_buffer_info.height = Settings().height;
        frame_buffer_info.layers = 1;

        VK_CHECK(vkCreateFramebuffer(m_device, &frame_buffer_info, nullptr, &m_shadow_frame_buffer));

        m_deletion_queue.Push([=]() {
            vkDestroyFramebuffer(m_device, m_shadow_frame_buffer, nullptr);
            });
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

        m_forward_pass_dsets.resize(Settings().max_frames_in_flight);
        m_instanced_dsets.resize(Settings().max_frames_in_flight);
        for (int i = 0; i < Settings().max_frames_in_flight; i++)
        {
            VkDescriptorBufferInfo scene_data_info = {};
            scene_data_info.buffer = m_scene_data_uniform_buffer.buffer;
            scene_data_info.offset = 0;
            scene_data_info.range = sizeof(SceneData);

            VkDescriptorBufferInfo dir_light_data_info = {};
            dir_light_data_info.buffer = m_directional_lights_buffer.buffer;
            dir_light_data_info.offset = 0;
            dir_light_data_info.range = VK_WHOLE_SIZE;

            VkDescriptorImageInfo shadow_map_info = {};
            shadow_map_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            shadow_map_info.imageView = m_shadow_pass_depth_texture_view;
            shadow_map_info.sampler = m_material_system->LinearSampler();

            VkDescriptorBufferInfo obj_data_info = {};
            obj_data_info.buffer = m_object_data_buffers[i].buffer;
            obj_data_info.offset = 0;
            obj_data_info.range = VK_WHOLE_SIZE;

            VkDescriptorBufferInfo instanced_data_info = {};
            instanced_data_info.buffer = m_instanced_data_buffers[i].buffer;
            instanced_data_info.offset = 0;
            instanced_data_info.range = VK_WHOLE_SIZE;

            // TODO: Stop descriptors bindings bound to all stages used by descriptor
            {
                DescriptorBuilder ds_builder = {};
                ds_builder.Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
                    .BindBuffer(SCENE_DATA_DESCRIPTOR_SET_BINDING, &scene_data_info, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                    .BindBuffer(DIRECTIONAL_LIGHTS_SET_BINDING, &dir_light_data_info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                    .BindImage(SHADOW_MAP_SET_BINDING, &shadow_map_info, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                    .BindBuffer(OBJECT_DATA_SET_BINDING, &obj_data_info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                    .Build(&m_forward_pass_dsets[i], &m_forward_pass_ds_layout);
            }

            // Create descriptor to hold RenderSceneIds for instanced drawing
            {
                DescriptorBuilder ds_builder = {};
                ds_builder.Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get())
                    .BindBuffer(INSTANCED_OBJECT_DATA_SET_BINDING, &instanced_data_info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                    .Build(&m_instanced_dsets[i], &m_instanced_ds_layout);
            }
        }
    }

    void VulkanRenderer::InitForwardPassFramebufffer()
    {
        VkImageView views[2] = { m_forward_pass_color_texture_view, m_forward_pass_depth_texture_view };

        VkFramebufferCreateInfo frame_buffer_info = {};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.attachmentCount = 2;
        frame_buffer_info.pAttachments = views;
        frame_buffer_info.renderPass = m_forward_pass;
        frame_buffer_info.width = Settings().width;
        frame_buffer_info.height = Settings().height;
        frame_buffer_info.layers = 1;

        VK_CHECK(vkCreateFramebuffer(m_device, &frame_buffer_info, nullptr, &m_forward_frame_buffer));

        m_deletion_queue.Push([=]() {
            vkDestroyFramebuffer(m_device, m_forward_frame_buffer, nullptr);
            });
    }

    void VulkanRenderer::InitSceneResources()
    {
        // Scene resources
        size_t padded_scene_data_size = VulkanResourceManager::PadToUniformBufferSize(sizeof(SceneData));
        size_t padded_directional_light_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(DirectionalLight));
        size_t padded_instanced_data_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(InstancedData));
        size_t padded_object_data_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(ObjectData));

        m_scene_data_uniform_buffer = VulkanResourceManager::CreateBuffer<SceneData>(padded_scene_data_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        m_directional_lights_buffer = VulkanResourceManager::CreateBuffer<DirectionalLight>(padded_directional_light_size * MAX_DIR_LIGHTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        m_object_data_buffers.resize(Settings().max_frames_in_flight);
        m_instanced_data_buffers.resize(Settings().max_frames_in_flight);
        for(int i = 0; i < Settings().max_frames_in_flight; i++)
        {
            m_object_data_buffers[i] = VulkanResourceManager::CreateBuffer<ObjectData>(padded_object_data_size * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            m_instanced_data_buffers[i] = VulkanResourceManager::CreateBuffer<InstancedData>(padded_instanced_data_size * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        }
    }

    void VulkanRenderer::InitForwardPassAttachments()
    {
        VkExtent3D extent = {};
        extent.width = Settings().width;
        extent.height = Settings().height;
        extent.depth = 1;

        m_forward_pass_color_texture = VulkanResourceManager::CreateImage(extent, m_forward_pass_format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        m_forward_pass_depth_texture = VulkanResourceManager::CreateImage(extent, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        VkImageViewCreateInfo color_info = vkinit::ImageViewCreateInfo(m_forward_pass_color_texture.image, m_forward_pass_format, VK_IMAGE_ASPECT_COLOR_BIT);
        VkImageViewCreateInfo depth_info = vkinit::ImageViewCreateInfo(m_forward_pass_depth_texture.image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

        VK_CHECK(vkCreateImageView(m_device, &color_info, nullptr, &m_forward_pass_color_texture_view));
        VK_CHECK(vkCreateImageView(m_device, &depth_info, nullptr, &m_forward_pass_depth_texture_view));

        // Create textures for swapchain
        Ref<VulkanTexture> viewport_texture = CreateRef<VulkanTexture>();
        viewport_texture->allocated_image = m_forward_pass_color_texture;
        viewport_texture->image_view = m_forward_pass_color_texture_view;
        viewport_texture->sampler = m_material_system->LinearSampler();
        m_viewport_texture = viewport_texture;

        m_deletion_queue.Push([=]() {
            vkDestroyImageView(m_device, m_forward_pass_depth_texture_view, nullptr);
            vkDestroyImageView(m_device, m_forward_pass_color_texture_view, nullptr);
            });
    }
};