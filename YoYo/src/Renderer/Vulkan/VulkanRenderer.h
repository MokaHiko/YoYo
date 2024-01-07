#pragma once
#include "Core/Memory.h"

#include "Renderer/Renderer.h"

#include "VulkanStructures.h"
#include "VulkanInitializers.h"
#include "VulkanUtils.h"

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
    private:
        void InitVulkan();
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructures();

        // TODO: Move to blit shader pass
        void InitBlitPipeline();
        void InitSwapchainRenderPass();
        void InitSwapchainFramebuffers();
        std::vector<VkFramebuffer> m_swapchain_framebuffers;
        VkRenderPass m_swapchain_render_pass;
        VkPipeline m_blit_pipeline;

        // TODO: Encapsulate as render targets
        VkRenderPass m_shadow_render_pass;
        VkRenderPass m_mesh_render_pass;
        VkRenderPass m_post_process_render_pass;

        void InitForwardPass();
    private:
        std::vector<VulkanFrameContext> m_frame_context;
        VulkanUploadContext m_upload_context;

        uint32_t m_swapchain_index = -1;
        int m_frame_count;
    private:
        VulkanQueues m_queues;
        VmaAllocator m_allocator;

        VkInstance m_instance;
        VkSurfaceKHR m_surface;

        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        VkPhysicalDevice m_physical_device;
        VkDevice m_device;

        VulkanDeletionQueue m_deletion_queue;
    };
}