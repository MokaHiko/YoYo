#pragma once
#include "Core/Memory.h"

#include "Renderer/Renderer.h"

#include "VulkanStructures.h"
#include "VulkanInitializers.h"
#include "VulkanUtils.h"
#include "VulkanMaterialSystem.h"

#include "VulkanResourceManager.h"
#include "VulkanMesh.h"

namespace yoyo
{
    struct VulkanRenderTarget
    {
        VkFramebuffer framebuffer;
        AllocatedImage image;
        uint32_t id;
    };

    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer();
        virtual ~VulkanRenderer();

        virtual void Init() override;
        virtual void Shutdown() override;

        virtual bool BeginFrame(const RenderPacket& rp) override;
        virtual void EndFrame() override;

        const VkDevice Device() const { return m_device;}
        const VkPhysicalDevice PhysicalDevice() const { return m_physical_device;}
        const VkInstance Instance() const {return m_instance;}

        VulkanQueues& Queues() {return m_queues;}
        VulkanDeletionQueue& DeletionQueue() {return m_deletion_queue;}
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
        Ref<VulkanMesh> blit_screen_mesh;

        // TODO: Encapsulate as render targets
        VkRenderPass m_shadow_render_pass;
        VkRenderPass m_post_process_render_pass;

        void InitForwardPass();
        VkRenderPass m_mesh_render_pass;
    private:
        std::vector<VulkanFrameContext> m_frame_context;

        uint32_t m_swapchain_index = -1;
        int m_frame_count;
    private:
        Ref<VulkanResourceManager> m_resource_manager;
    private:
        VulkanQueues m_queues;

        VkInstance m_instance;
        VkSurfaceKHR m_surface;

        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        VkPhysicalDevice m_physical_device;
        VkDevice m_device;

        // Descriptor allocator and layout cache
		Ref<DescriptorAllocator> m_descriptor_allocator;
		Ref<DescriptorLayoutCache> m_descriptor_layout_cache;

        VulkanDeletionQueue m_deletion_queue;
    };
}