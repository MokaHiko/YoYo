#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <Renderer/RenderTypes.h>

namespace yoyo
{
    struct VulkanRenderContext
    {
        // The recording command buffer
        VkCommandBuffer cmd;

        // The current pipeline layout
        VkPipelineLayout pipeline_layout; 

        // The current pass
        MeshPassType mesh_pass_type; 

        // Current frame
        uint32_t frame;
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

    struct VulkanShaderModule
    {
        VkShaderModule module;
        std::vector<uint32_t> code;
        std::string source_path;
    };

    // A struct that holds a handle to buffer and its allocation
    template<typename T = void>
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

    enum class VulkanBindingPropertType
    {
        Int32,
        Float32,
        Vec4,
        Texture,
    };

    struct VulkanBinding
    {
        std::string name;
        VkDescriptorType type;

        struct BindingProperty
        {
            uint64_t size;    // size of binding property member in bytes
            uint64_t offset;  // offset of binding property member in bytes
            VulkanBindingPropertType type;
        };

        // Size of binding in bytes
        const uint64_t Size() const
        {
            return m_size;
        }

        void AddProperty(const std::string& name, const BindingProperty& binding_prop)
        {
            m_properties[name] = binding_prop;
            m_size += binding_prop.size;
        }

        const std::unordered_map<std::string, BindingProperty>& Properties()
        {
            return m_properties;
        }
    private:
        uint64_t m_size;  
        std::unordered_map<std::string, BindingProperty> m_properties;
    };

    struct VulkanDescriptorSetInformation
    {
        VkShaderStageFlagBits shader_stage;
        VkDescriptorSetLayout descriptor_set_layout;

        uint32_t index;
        std::unordered_map<uint32_t, VulkanBinding> bindings;

        void AddBinding(uint32_t index, VkShaderStageFlagBits stage, const VulkanBinding& binding)
        {
            // Check if binding already exists
            if(bindings.find(index) != bindings.end())
            {
                shader_stage = static_cast<VkShaderStageFlagBits>(shader_stage | stage);
            }
            else
            {
                bindings[index] = binding;
            }
        }
    };

    struct VulkanDescriptorSet
    {
        VkDescriptorSet set;
        VulkanDescriptorSetInformation info;
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