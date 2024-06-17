#pragma once
#include "Core/Memory.h"

#include "Renderer/Renderer.h"

#include "VulkanStructures.h"
#include "VulkanInitializers.h"
#include "VulkanUtils.h"

namespace yoyo
{
    const int FRAMES_IN_FLIGHT = 2;

    const int SCENE_DATA_DESCRIPTOR_SET_INDEX = 1;
    const int SCENE_DATA_DESCRIPTOR_SET_BINDING = 0;

    const int DIRECTIONAL_LIGHTS_SET_INDEX = 0;
    const int DIRECTIONAL_LIGHTS_SET_BINDING = 1;

    const int SHADOW_MAP_SET_INDEX = 0;
    const int SHADOW_MAP_SET_BINDING = 4;

    const int OBJECT_DATA_SET_INDEX = 0;
    const int OBJECT_DATA_SET_BINDING = 5;

    const int INSTANCED_OBJECT_DATA_SET_INDEX = 3;
    const int INSTANCED_OBJECT_DATA_SET_BINDING = 0;

    const int SHADOW_PASS_INSTANCED_OBJECT_DATA_SET_INDEX = 1;
    const int SHADOW_PASS_INSTANCED_OBJECT_DATA_SET_BINDING = 0;
    
    class Texture;
    class DirectionalLight;
    class Camera;
    class StaticMesh;

    class VulkanMaterialSystem;
    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer();
        virtual ~VulkanRenderer();

        virtual void* RenderContext() override;

        virtual void Init() override;
        virtual void Shutdown() override;

        virtual const uint32_t GetCurrentFrame() const override {return m_frame_count;}
        virtual const Ref<Texture>&  GetViewPortTexture() const override {return m_viewport_texture;} // The render context of the current frame 

        virtual bool BeginFrame(const Ref<RenderScene> scene) override;

        virtual void BeginBlitPass() override;
        virtual void EndBlitPass() override;

        virtual void EndFrame() override;

        const VkDevice Device() const { return m_device; }
        const VkPhysicalDevice PhysicalDevice() const { return m_physical_device; }
        const VkPhysicalDeviceProperties& PhysicalDeviceProperties() const { return m_physical_device_properties; }
        const VkInstance Instance() const { return m_instance; }

        VulkanQueues& Queues() { return m_queues; }
        VulkanDeletionQueue& DeletionQueue() { return m_deletion_queue; }

        Ref<DescriptorAllocator> DescAllocator() { return m_descriptor_allocator; }
        Ref<DescriptorLayoutCache> DescLayoutCache() { return m_descriptor_layout_cache; }

        VkRenderPass SwapChainRenderPass() {return m_swapchain_render_pass;}
    private:
        void InitVulkan();
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructures();

        // TODO: Move to blit shader pass
        void InitBlitPipeline();
        void InitSwapchainRenderPass();
        void InitSwapchainFramebuffers();
        VkPipelineLayout m_blit_pipeline_layout;
        VkPipeline m_blit_pipeline;

        std::vector<VkFramebuffer> m_swapchain_framebuffers;
        VkRenderPass m_swapchain_render_pass;
        Ref<Texture> m_viewport_texture;

        VkDescriptorSet m_blit_output_texture_ds;
        VkDescriptorSetLayout m_blit_pass_ds_layout;

        Ref<StaticMesh> m_screen_quad;

        // TODO: Encapsulate as render targets
        VkRenderPass m_post_process_render_pass;
        
        // Shadow Pass
        void InitShadowPass();
        void InitShadowPassAttachments();
        void InitShadowPassFramebufffer();

        VkRenderPass m_shadow_render_pass;
        AllocatedImage m_shadow_pass_depth_texture;
        VkImageView m_shadow_pass_depth_texture_view;

        VkFramebuffer m_shadow_frame_buffer;

        std::vector<VkDescriptorSet> m_shadow_pass_dsets;
        VkDescriptorSetLayout m_shadow_pass_ds_layout;

        // Forward Pass
        struct SceneData
        {
            Mat4x4 view;
            Mat4x4 proj;

            uint32_t dir_light_count;
            uint32_t point_light_count;
            uint32_t spot_light_count;
            uint32_t area_light_count;
        };

        struct ObjectData
        {
            Mat4x4 model_matrix;
            Vec4 color;
        };

        struct InstancedData
        {
            RenderSceneId id;
        };

        void InitForwardPass();
        void InitForwardPassAttachments();
        void InitForwardPassFramebufffer();
        VkFormat m_forward_pass_format = VK_FORMAT_B8G8R8A8_UNORM;
        VkFormat m_depth_pass_format = VK_FORMAT_D32_SFLOAT_S8_UINT;

        AllocatedImage m_forward_pass_color_texture;
        AllocatedImage m_forward_pass_depth_texture;
        VkImageView m_forward_pass_color_texture_view;
        VkImageView m_forward_pass_depth_texture_view;

        VkFramebuffer m_forward_frame_buffer;
        VkRenderPass m_forward_pass;
    private:
        // Scene Resources
        AllocatedBuffer<SceneData> m_scene_data_uniform_buffer;
        AllocatedBuffer<DirectionalLight> m_directional_lights_buffer;

        std::vector<AllocatedBuffer<ObjectData>> m_object_data_buffers;
        std::vector<VkDescriptorSet> m_forward_pass_dsets;
        VkDescriptorSetLayout m_forward_pass_ds_layout;

        std::vector<AllocatedBuffer<InstancedData>> m_instanced_data_buffers;
        std::vector<VkDescriptorSet> m_instanced_dsets;
        VkDescriptorSetLayout m_instanced_ds_layout;

        void InitSceneResources();

        VulkanRenderContext m_render_context;
    private:
        std::vector<VulkanFrameContext> m_frame_context;

        uint32_t m_swapchain_index = -1;
        int m_frame_count;
    private:
        Ref<VulkanMaterialSystem> m_material_system;
    private:
        VulkanQueues m_queues;

        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
        VkSurfaceKHR m_surface;

        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        VkPhysicalDevice m_physical_device;
        VkPhysicalDeviceProperties m_physical_device_properties;
        VkDevice m_device;

        // TODO: Delete and make one for each subsystem to vulkan material system
        // Descriptor allocator and layout cache
        Ref<DescriptorAllocator> m_descriptor_allocator;
        Ref<DescriptorLayoutCache> m_descriptor_layout_cache;

        VulkanDeletionQueue m_deletion_queue;
    };
}