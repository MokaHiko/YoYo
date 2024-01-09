#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace yoyo
{
    struct VulkanRenderContext
    {
        VkCommandBuffer cmd;
    };

    struct VulkanQueue
    {
        VkQueue queue;
        uint32_t index;
    };

    struct VulkanQueues
    {
        VulkanQueue graphics;
        VulkanQueue transfer;
        VulkanQueue compute;
    };

    struct VulkanUploadContext
    {
        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;
        VkFence fence;
    };

    struct VulkanFrameContext
    {
        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;

        VkCommandPool transfer_command_pool;

        VkCommandPool compute_command_pool;
        VkCommandBuffer compute_command_buffer;

        VkSemaphore present_semaphore, render_semaphore;
        VkFence render_fence;
    };

    // A struct that holds a handle to buffer and its allocation
    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
    };

    // A struct that holds a handle to image and a allocation
    struct AllocatedImage
    {
        VkImage image;
        VmaAllocation allocation;
    };

    struct VulkanDescriptorSet
    {
        uint32_t set_index;
        VkShaderStageFlagBits shader_stage;
        VkDescriptorSetLayout descriptor_set_layout;
        std::unordered_map<uint32_t, VkDescriptorType> bindings;

        void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlagBits stage)
        {
            bindings[binding] = type;
            shader_stage = static_cast<VkShaderStageFlagBits>(shader_stage | stage);
        }
    };

    // Queue that keeps clean up functions
    struct VulkanDeletionQueue
    {
        // Pushes and takes ownership of callback in deletion queue for clean up in FIFO order
        void Push(std::function<void()> &&fn)
        {
            clean_functions.push_back(fn);
        }

        // Calls all clean up functions in FIFO order
        void Flush()
        {
            for (auto it = clean_functions.rbegin(); it != clean_functions.rend(); it++)
            {
                (*it)();
            }

            clean_functions.clear();
        }

    private:
        std::deque<std::function<void()>> clean_functions = {};
    };
}