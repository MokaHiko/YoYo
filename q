[1mdiff --git a/CMakeLists.txt b/CMakeLists.txt[m
[1mindex ec29927..f605e54 100644[m
[1m--- a/CMakeLists.txt[m
[1m+++ b/CMakeLists.txt[m
[36m@@ -1,4 +1,4 @@[m
[31m-cmake_minimum_required(VERSION 3.0.0)[m
[32m+[m[32mcmake_minimum_required(VERSION 3.5.0)[m
 project(YoYo VERSION 0.1.0 LANGUAGES C CXX)[m
 [m
 include(CTest)[m
[36m@@ -93,8 +93,8 @@[m [madd_library(YoYo SHARED[m
     src/Renderer/Particles/ParticleSystemEvents.h[m
     src/Renderer/Particles/ParticleSystemEvents.cpp[m
 [m
[31m-    src/Renderer/Voxel/BlockMesh.h[m
[31m-    src/Renderer/Voxel/BlockMesh.cpp[m
[32m+[m[32m    # src/Renderer/Voxel/BlockMesh.h[m
[32m+[m[32m    # src/Renderer/Voxel/BlockMesh.cpp[m
 [m
     src/Core/EntryPoint.h[m
     include/Yoyo.h[m
[36m@@ -147,7 +147,7 @@[m [mif (WIN32)[m
     target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 vulkan-1)[m
 [m
     target_include_directories(${PROJECT_NAME} PRIVATE vendor/SDL/include $ENV{VULKAN_SDK}/include)[m
[31m-    target_link_directories(${PROJECT_NAME} PRIVATE $ENV{VULKAN_SDK}/Lib)[m
[32m+[m[32m    target_link_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Lib)[m
 [m
     set(YRENDERERTYPE "VULKAN")[m
 elseif (APPLE)[m
[36m@@ -171,7 +171,7 @@[m [mif (YRENDERERTYPE STREQUAL "VULKAN")[m
     add_subdirectory(vendor/SPIRVReflect)[m
 [m
     # TODO: Make spriv reflect static fork for static lib[m
[31m-    target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::VulkanMemoryAllocator vk-bootstrap spirv-reflect-static)[m
[32m+[m[32m    target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator vk-bootstrap spirv-reflect-static)[m
     target_sources(${PROJECT_NAME} PRIVATE [m
         src/Renderer/Vulkan/VulkanStructures.cpp[m
         src/Renderer/Vulkan/VulkanStructures.h[m
[1mdiff --git a/src/Events/Event.cpp b/src/Events/Event.cpp[m
[1mindex 7da75f8..5fc94c2 100644[m
[1m--- a/src/Events/Event.cpp[m
[1m+++ b/src/Events/Event.cpp[m
[36m@@ -4,13 +4,25 @@[m
 [m
 namespace yoyo[m
 {[m
[32m+[m	[32mEventManager &EventManager::Instance()[m
[32m+[m	[32m{[m
[32m+[m		[32mstatic EventManager *event_manager;[m
[32m+[m
[32m+[m		[32mif (!event_manager)[m
[32m+[m		[32m{[m
[32m+[m			[32mevent_manager = YNEW EventManager;[m
[32m+[m		[32m}[m
[32m+[m
[32m+[m		[32mreturn *event_manager;[m
[32m+[m	[32m}[m
[32m+[m
 	// TODO: Return handler[m
[31m-	void EventManager::Subscribe(const EventType& type, const EventHandler& handler)[m
[32m+[m	[32mvoid EventManager::Subscribe(const EventType &type, const EventHandler &handler)[m
 	{[m
[31m-		auto& it = m_event_listeners[type];[m
[32m+[m		[32mauto &it = m_event_listeners[type];[m
 [m
 		// TODO: Change and return EventHandler to struct with id[m
[31m-		//auto some_thing = std::find(it.begin(), it.end(), handler);[m
[32m+[m		[32m// auto some_thing = std::find(it.begin(), it.end(), handler);[m
 		// if(std::find(it.begin(), it.end(), handler) != it.end())[m
 		// {[m
 		// 	YINFO("EventHandler of type %d already registerd!");[m
[36m@@ -21,16 +33,16 @@[m [mnamespace yoyo[m
 		m_event_listeners[type].push_back(std::move(handler));[m
 	}[m
 [m
[31m-	void EventManager::Unsubscribe(const EventType& type, EventHandler& handler)[m
[32m+[m	[32mvoid EventManager::Unsubscribe(const EventType &type, EventHandler &handler)[m
 	{[m
 		// TODO: unsub[m
 	}[m
 [m
 	void EventManager::Dispatch(Ref<Event> event)[m
 	{[m
[31m-		for(EventHandler& event_handler : m_event_listeners[event->Type()])[m
[32m+[m		[32mfor (EventHandler &event_handler : m_event_listeners[event->Type()])[m
 		{[m
[31m-			if(event_handler(event))[m
[32m+[m			[32mif (event_handler(event))[m
 			{[m
 				return;[m
 			}[m
[1mdiff --git a/src/Events/Event.h b/src/Events/Event.h[m
[1mindex a97812e..1f1771a 100644[m
[1m--- a/src/Events/Event.h[m
[1m+++ b/src/Events/Event.h[m
[36m@@ -28,17 +28,7 @@[m [mnamespace yoyo[m
     class YAPI EventManager[m
     {[m
     public:[m
[31m-        static EventManager& Instance()[m
[31m-        {[m
[31m-            static EventManager* event_manager;[m
[31m-[m
[31m-            if (!event_manager)[m
[31m-            {[m
[31m-                event_manager = YNEW EventManager;[m
[31m-            }[m
[31m-[m
[31m-            return *event_manager;[m
[31m-        }[m
[32m+[m[32m        static EventManager& Instance();[m
 [m
         void Subscribe(const EventType& type, const EventHandler& handler);[m
         void Unsubscribe(const EventType& type, EventHandler& handler);[m
[1mdiff --git a/src/Math/Math.cpp b/src/Math/Math.cpp[m
[1mindex 35d1286..db9c2b3 100644[m
[1m--- a/src/Math/Math.cpp[m
[1m+++ b/src/Math/Math.cpp[m
[36m@@ -91,6 +91,16 @@[m [mnamespace yoyo[m
         return out;[m
     }[m
 [m
[32m+[m[32m    YAPI const IVec3 operator*(const IVec3 &v1, int scalar)[m
[32m+[m[32m    {[m
[32m+[m[32m        IVec3 out = {};[m
[32m+[m[32m        out.x = v1.x * scalar;[m
[32m+[m[32m        out.y = v1.y * scalar;[m
[32m+[m[32m        out.z = v1.z * scalar;[m
[32m+[m
[32m+[m[32m        return out;[m
[32m+[m[32m    }[m
[32m+[m
     const Vec3 operator*(const Vec3 &v1, float scalar)[m
     {[m
         Vec3 out = {};[m
[1mdiff --git a/src/Math/Math.h b/src/Math/Math.h[m
[1mindex bf92df4..90697b2 100644[m
[1m--- a/src/Math/Math.h[m
[1m+++ b/src/Math/Math.h[m
[36m@@ -50,6 +50,7 @@[m [mnamespace yoyo[m
     YAPI const IVec2 operator-(const IVec2& v1, const IVec2& v2);[m
 [m
     YAPI const IVec2 operator*(const IVec2& v1, int scalar);[m
[32m+[m[32m    YAPI const IVec3 operator*(const IVec3& v1, int scalar);[m
     YAPI const Vec3 operator*(const Vec3& v1, float scalar);[m
     YAPI const Vec4 operator*(const Vec4& v1, float scalar);[m
 [m
[1mdiff --git a/src/Renderer/RenderTypes.h b/src/Renderer/RenderTypes.h[m
[1mindex 27aff12..91b9a2e 100644[m
[1m--- a/src/Renderer/RenderTypes.h[m
[1m+++ b/src/Renderer/RenderTypes.h[m
[36m@@ -2,6 +2,9 @@[m
 [m
 namespace yoyo[m
 {[m
[32m+[m[32m    // Render pass[m
[32m+[m[32m    using ShaderPassId = uint64_t;[m
[32m+[m[41m    [m
     // Unique identifier of a renderable object in a RenderScene.[m
     using RenderSceneId = uint32_t;[m
     const RenderSceneId NULL_RENDER_SCENE_ID = -1;[m
[36m@@ -15,6 +18,7 @@[m [mnamespace yoyo[m
         ForwardTransparent,[m
         Shadow,[m
         PostProcess,[m
[32m+[m[32m        Compute,[m
 [m
         Max,[m
     };[m
[1mdiff --git a/src/Renderer/Shader.h b/src/Renderer/Shader.h[m
[1mindex 64a5d88..68b988f 100644[m
[1m--- a/src/Renderer/Shader.h[m
[1m+++ b/src/Renderer/Shader.h[m
[36m@@ -13,7 +13,7 @@[m [mnamespace yoyo[m
 [m
     struct ShaderPass[m
     {[m
[31m-        [m
[32m+[m[32m        ShaderPassId id;[m
     };[m
 [m
     struct ShaderInput[m
[1mdiff --git a/src/Renderer/Vulkan/VulkanMaterialSystem.cpp b/src/Renderer/Vulkan/VulkanMaterialSystem.cpp[m
[1mindex db5b65a..f863eb0 100644[m
[1m--- a/src/Renderer/Vulkan/VulkanMaterialSystem.cpp[m
[1m+++ b/src/Renderer/Vulkan/VulkanMaterialSystem.cpp[m
[36m@@ -116,7 +116,7 @@[m [mnamespace yoyo[m
                         uint32_t binding_index = 0;[m
 [m
                         size_t padded_material_property_size = VulkanResourceManager::PadToUniformBufferSize(binding.Size());[m
[31m-                        material->m_properties_buffer = VulkanResourceManager::CreateBuffer(padded_material_property_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);[m
[32m+[m[32m                        material->m_properties_buffer = VulkanResourceManager::CreateBuffer<>(padded_material_property_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);[m
 [m
                         // 0 out property buffer[m
                         void *property_data;[m
[1mdiff --git a/src/Renderer/Vulkan/VulkanMesh.cpp b/src/Renderer/Vulkan/VulkanMesh.cpp[m
[1mindex c76d4cc..f973a0a 100644[m
[1m--- a/src/Renderer/Vulkan/VulkanMesh.cpp[m
[1m+++ b/src/Renderer/Vulkan/VulkanMesh.cpp[m
[36m@@ -1,7 +1,7 @@[m
 #include "VulkanMesh.h"[m
 [m
[31m-#include "Resource/ResourceEvent.h"[m
 #include "VulkanResourceManager.h"[m
[32m+[m[32m#include "Resource/ResourceEvent.h"[m
 [m
 namespace yoyo[m
 {[m
[36m@@ -118,6 +118,11 @@[m [mnamespace yoyo[m
         return attributes;[m
     }[m
 [m
[32m+[m[32m    void YAPI DispatchMeshCreatedEvent(Ref<IMesh> mesh)[m
[32m+[m[32m    {[m
[32m+[m[32m        EventManager::Instance().Dispatch(CreateRef<MeshCreatedEvent<IMesh>>(mesh));[m
[32m+[m[32m    }[m
[32m+[m
     Ref<SkinnedMesh> SkinnedMesh::Create(const std::string &name)[m
     {[m
         Ref<VulkanSkinnedMesh> mesh = CreateRef<VulkanSkinnedMesh>();[m
[36m@@ -167,7 +172,9 @@[m [mnamespace yoyo[m
 [m
     void VulkanSkinnedMesh::UploadMeshData(bool free_host_memory)[m
     {[m
[31m-        VulkanResourceManager::UploadMesh<VulkanSkinnedMesh>(this);[m
[32m+[m[32m        return;[m
[32m+[m[32m        // TODO: Implement skinned mesh[m
[32m+[m[32m        // VulkanResourceManager::UploadMesh<VulkanSkinnedMesh>(this);[m
 [m
         if (free_host_memory)[m
         {[m
[1mdiff --git a/src/Renderer/Vulkan/VulkanMesh.h b/src/Renderer/Vulkan/VulkanMesh.h[m
[1mindex 960d9f2..489dbc4 100644[m
[1m--- a/src/Renderer/Vulkan/VulkanMesh.h[m
[1m+++ b/src/Renderer/Vulkan/VulkanMesh.h[m
[36m@@ -7,8 +7,6 @@[m
 #include "VulkanStructures.h"[m
 #include "VulkanResourceManager.h"[m
 [m
[31m-#include "Resource/ResourceEvent.h"[m
[31m-[m
 namespace yoyo[m
 {[m
     const std::vector<VkVertexInputAttributeDescription> &VertexAttributeDescriptions();[m
[36m@@ -17,8 +15,18 @@[m [mnamespace yoyo[m
     std::vector<VkVertexInputBindingDescription> GenerateVertexBindingDescriptions(const std::vector<ShaderInput> &inputs, VkVertexInputRate input_rate);[m
     std::vector<VkVertexInputAttributeDescription> GenerateVertexAttributeDescriptions(const std::vector<ShaderInput> &inputs);[m
 [m
[32m+[m[32m    void YAPI DispatchMeshCreatedEvent(Ref<IMesh> mesh);[m
[32m+[m
[32m+[m[32m    template <typename VertexType, typename IndexType>[m
[32m+[m[32m    class VulkanMesh : public Mesh<VertexType, IndexType>[m
[32m+[m[32m    {[m
[32m+[m[32m    public:[m
[32m+[m[32m        AllocatedBuffer<> vertex_buffer = {};[m
[32m+[m[32m        AllocatedBuffer<uint32_t> index_buffer = {};[m
[32m+[m[32m    };[m
[32m+[m
     template <typename VertexType, typename IndexType>[m
[31m-    class VulkanStaticMesh : public Mesh<VertexType, IndexType>[m
[32m+[m[32m    class VulkanStaticMesh : public VulkanMesh<VertexType, IndexType>[m
     {[m
     public:[m
         VulkanStaticMesh() = default;[m
[36m@@ -41,7 +49,7 @@[m [mnamespace yoyo[m
 [m
         virtual void UploadMeshData(bool free_host_memory = false) override[m
         {[m
[31m-            VulkanResourceManager::UploadMesh(this);[m
[32m+[m[32m            VulkanResourceManager::UploadMesh<VertexType, IndexType>(this);[m
 [m
             if (free_host_memory)[m
             {[m
[36m@@ -49,12 +57,8 @@[m [mnamespace yoyo[m
                 indices.clear();[m
             }[m
 [m
[31m-            RemoveDirtyFlags(MeshDirtyFlags::Unuploaded);[m
[32m+[m[32m            RemoveDirtyFlags(MeshDirtyFlags::Unuploaded | MeshDirtyFlags::IndexDataChange | MeshDirtyFlags::VertexDataChange);[m
         }[m
[31m-[m
[31m-    public:[m
[31m-        AllocatedBuffer<> vertex_buffer = {};[m
[31m-        AllocatedBuffer<uint32_t> index_buffer = {};[m
     };[m
 [m
     template <typename VertexType, typename IndexType>[m
[36m@@ -63,7 +67,8 @@[m [mnamespace yoyo[m
         auto mesh = CreateRef<VulkanStaticMesh<VertexType, IndexType>>();[m
         mesh->name = name;[m
 [m
[31m-        EventManager::Instance().Dispatch(CreateRef<MeshCreatedEvent<IMesh>>(mesh));[m
[32m+[m[32m        DispatchMeshCreatedEvent(mesh);[m
[32m+[m
         return mesh;[m
     }[m
 [m
[1mdiff --git a/src/Renderer/Vulkan/VulkanRenderer.cpp b/src/Renderer/Vulkan/VulkanRenderer.cpp[m
[1mindex 8c45743..716831e 100644[m
[1m--- a/src/Renderer/Vulkan/VulkanRenderer.cpp[m
[1m+++ b/src/Renderer/Vulkan/VulkanRenderer.cpp[m
[36m@@ -282,24 +282,6 @@[m [mnamespace yoyo[m
             shader->shader_passes[MeshPassType::Forward] = shader_pass;[m
         }[m
 #endif[m
[31m-        // TODO: Move to Game Layer[m
[31m-        // Global Block Shader[m
[31m-        {[m
[31m-            Ref<VulkanShaderEffect> lit_block_effect = CreateRef<VulkanShaderEffect>();[m
[31m-            {[m
[31m-                Ref<VulkanShaderModule> vertex_module = VulkanResourceManager::CreateShaderModule("assets/shaders/lit_block_shader.vert.spv");[m
[31m-                lit_block_effect->PushShader(vertex_module, VK_SHADER_STAGE_VERTEX_BIT);[m
[31m-[m
[31m-                Ref<VulkanShaderModule> fragment_module = VulkanResourceManager::CreateShaderModule("assets/shaders/lit_block_shader.frag.spv");[m
[31m-                lit_block_effect->PushShader(fragment_module, VK_SHADER_STAGE_FRAGMENT_BIT);[m
[31m-            }[m
[31m-            lit_block_effect->primitive_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;[m
[31m-            auto lit_block_shader_pass = m_material_system->CreateShaderPass(m_forward_pass, lit_block_effect);[m
[31m-[m
[31m-            Ref<Shader> lit_block_shader = Shader::Create("lit_block_shader");[m
[31m-            lit_block_shader->shader_passes[MeshPassType::Forward] = lit_block_shader_pass;[m
[31m-            lit_block_shader->shader_passes[MeshPassType::Shadow] = shadow_pass;[m
[31m-        }[m
     }[m
 [m
     void VulkanRenderer::Shutdown()[m
[36m@@ -555,8 +537,7 @@[m [mnamespace yoyo[m
             rect.extent.height = Settings().height;[m
 [m
             VkClearValue color_clear = {};[m
[31m-            // color_clear.color = { 0.0f, 1.0f, 1.0f, 1.0f };[m
[31m-            color_clear.color = {0.0f, 0.0f, 0.0f, 0.0f};[m
[32m+[m[32m            color_clear.color = {0.25f, 0.25f, 0.25f, 1.0f};[m
             VkClearValue depth_clear = {};[m
             depth_clear.depthStencil = {1.0f, 0};[m
 [m
[1mdiff --git a/src/Renderer/Vulkan/VulkanResourceManager.cpp b/src/Renderer/Vulkan/VulkanResourceManager.cpp[m
[1mindex 7b47764..e619cc6 100644[m
[1m--- a/src/Renderer/Vulkan/VulkanResourceManager.cpp[m
[1m+++ b/src/Renderer/Vulkan/VulkanResourceManager.cpp[m
[36m@@ -11,6 +11,22 @@[m
 [m
 namespace yoyo[m
 {[m
[32m+[m	[32m// General purpose staging buffer for mesh uploads[m
[32m+[m[32m    static AllocatedBuffer<> s_mesh_staging_buffer = {};[m[41m [m
[32m+[m[32m    static VulkanUploadContext s_upload_context = {};[m
[32m+[m
[32m+[m[32m    static VkDevice s_device = VK_NULL_HANDLE;[m
[32m+[m[32m    static VkPhysicalDeviceProperties s_physical_device_props = {};[m
[32m+[m[32m    static VkPhysicalDevice s_physical_device = VK_NULL_HANDLE;[m
[32m+[m[32m    static VkInstance s_instance = VK_NULL_HANDLE;[m
[32m+[m
[32m+[m[32m    static VulkanDeletionQueue *s_deletion_queue = nullptr;[m
[32m+[m[32m    static VulkanQueues *s_queues = nullptr;[m
[32m+[m
[32m+[m[32m    static VmaAllocator s_allocator = VK_NULL_HANDLE;[m
[32m+[m[32m    static Ref<DescriptorAllocator> s_descriptor_allocator = nullptr;[m
[32m+[m[32m    static Ref<DescriptorLayoutCache> s_descriptor_layout_cache = nullptr;[m
[32m+[m
 	static VkFormat ConvertTextureFormat(TextureFormat format)[m
 	{[m
 		switch (format)[m
[36m@@ -32,44 +48,44 @@[m [mnamespace yoyo[m
 	void VulkanResourceManager::Init(VulkanRenderer *renderer)[m
 	{[m
 		// Grab Handles[m
[31m-		m_device = renderer->Device();[m
[31m-		m_physical_device = renderer->PhysicalDevice();[m
[31m-		m_physical_device_props = renderer->PhysicalDeviceProperties();[m
[31m-		m_instance = renderer->Instance();[m
[32m+[m		[32ms_device = renderer->Device();[m
[32m+[m		[32ms_physical_device = renderer->PhysicalDevice();[m
[32m+[m		[32ms_physical_device_props = renderer->PhysicalDeviceProperties();[m
[32m+[m		[32ms_instance = renderer->Instance();[m
 [m
 		// TODO: Create cache and allocator for resource manager[m
[31m-		m_descriptor_allocator = renderer->DescAllocator();[m
[31m-		m_descriptor_layout_cache = renderer->DescLayoutCache();[m
[32m+[m		[32ms_descriptor_allocator = renderer->DescAllocator();[m
[32m+[m		[32ms_descriptor_layout_cache = renderer->DescLayoutCache();[m
 [m
[31m-		m_queues = &renderer->Queues();[m
[31m-		m_deletion_queue = &renderer->DeletionQueue();[m
[32m+[m		[32ms_queues = &renderer->Queues();[m
[32m+[m		[32ms_deletion_queue = &renderer->DeletionQueue();[m
 [m
 		// Init vmaAllocator[m
 		VmaAllocatorCreateInfo allocator_info = {};[m
[31m-		allocator_info.physicalDevice = m_physical_device;[m
[31m-		allocator_info.device = m_device;[m
[31m-		allocator_info.instance = m_instance;[m
[32m+[m		[32mallocator_info.physicalDevice = s_physical_device;[m
[32m+[m		[32mallocator_info.device = s_device;[m
[32m+[m		[32mallocator_info.instance = s_instance;[m
 [m
[31m-		VK_CHECK(vmaCreateAllocator(&allocator_info, &m_allocator));[m
[32m+[m		[32mVK_CHECK(vmaCreateAllocator(&allocator_info, &s_allocator));[m
 [m
 		// Create immediate submit context[m
 		{[m
 			VkCommandPoolCreateInfo graphics_cp_info = vkinit::CommandPoolCreateInfo(renderer->Queues().graphics.index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);[m
 [m
[31m-			vkCreateCommandPool(m_device, &graphics_cp_info, nullptr, &m_upload_context.command_pool);[m
[31m-			VkCommandBufferAllocateInfo alloc_info = vkinit::CommandBufferAllocInfo(m_upload_context.command_pool);[m
[31m-			vkAllocateCommandBuffers(m_device, &alloc_info, &m_upload_context.command_buffer);[m
[32m+[m			[32mvkCreateCommandPool(s_device, &graphics_cp_info, nullptr, &s_upload_context.command_pool);[m
[32m+[m			[32mVkCommandBufferAllocateInfo alloc_info = vkinit::CommandBufferAllocInfo(s_upload_context.command_pool);[m
[32m+[m			[32mvkAllocateCommandBuffers(s_device, &alloc_info, &s_upload_context.command_buffer);[m
 [m
 			VkFenceCreateInfo fence_info = {};[m
 			fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;[m
 			fence_info.flags = 0;[m
 [m
[31m-			VK_CHECK(vkCreateFence(m_device, &fence_info, nullptr, &m_upload_context.fence));[m
[32m+[m			[32mVK_CHECK(vkCreateFence(s_device, &fence_info, nullptr, &s_upload_context.fence));[m
 		}[m
 [m
 		// Init staging buffers[m
 		{[m
[31m-			m_mesh_staging_buffer = CreateBuffer(sizeof(Vertex) * MAX_MESH_VERTICES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);[m
[32m+[m			[32ms_mesh_staging_buffer = CreateBuffer<>(STAGING_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);[m
 		}[m
 [m
 		// Subscribe to vulkan resource events[m
[36m@@ -82,20 +98,36 @@[m [mnamespace yoyo[m
 		// TODO: Unsubscribe to resource events[m
 [m
 		// Destroy immediate upload context[m
[31m-		vkDestroyCommandPool(m_device, m_upload_context.command_pool, nullptr);[m
[31m-		vkDestroyFence(m_device, m_upload_context.fence, nullptr);[m
[32m+[m		[32mvkDestroyCommandPool(s_device, s_upload_context.command_pool, nullptr);[m
[32m+[m		[32mvkDestroyFence(s_device, s_upload_context.fence, nullptr);[m
 [m
 		// Destroy allocator[m
[31m-		vmaDestroyAllocator(m_allocator);[m
[32m+[m		[32mvmaDestroyAllocator(s_allocator);[m
 	}[m
 [m
[31m-	bool VulkanResourceManager::UploadTexture(VulkanTexture *texture)[m
[32m+[m[32m    AllocatedBuffer<> &VulkanResourceManager::StagingBuffer()[m
[32m+[m[32m    {[m
[32m+[m		[32mreturn s_mesh_staging_buffer;[m
[32m+[m[32m    }[m
[32m+[m
[32m+[m	[32mconst VmaAllocator VulkanResourceManager::Allocator()[m
 	{[m
[32m+[m		[32mreturn s_allocator;[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m[32m    VulkanDeletionQueue &VulkanResourceManager::DeletionQueue()[m
[32m+[m[32m    {[m
[32m+[m		[32mYASSERT(s_deletion_queue != nullptr);[m
[32m+[m		[32mreturn *s_deletion_queue;[m
[32m+[m[32m    }[m
[32m+[m
[32m+[m[32m    bool VulkanResourceManager::UploadTexture(VulkanTexture *texture)[m
[32m+[m[32m    {[m
 		YASSERT(texture->layers <= 1 != ((texture->GetTextureType() & TextureType::Array) == TextureType::Array),[m
 				"Cannot have layer count greater than 1 that is not TextureType::Array or TextureType::CubeMap!");[m
 [m
 		// Copy to staging buffer[m
[31m-		AllocatedBuffer staging_buffer = CreateBuffer(texture->raw_data.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, false);[m
[32m+[m		[32mAllocatedBuffer<uint8_t> staging_buffer = CreateBuffer<uint8_t>(texture->raw_data.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, false);[m
 [m
 		void *pixel_ptr;[m
 		MapMemory(staging_buffer.allocation, &pixel_ptr);[m
[36m@@ -103,8 +135,6 @@[m [mnamespace yoyo[m
 		UnmapMemory(staging_buffer.allocation);[m
 [m
 		YASSERT(texture->layers > 0, "Cannot have texture with 0 layers!");[m
[31m-		YASSERT(texture->width %  texture->layers == 0, "Multiple layer texture data is not square");[m
[31m-		YASSERT(texture->height % texture->layers == 0, "Multiple layer texture data is not square");[m
 [m
 		// Create texture[m
 		VkExtent3D extent = {};[m
[36m@@ -121,7 +151,8 @@[m [mnamespace yoyo[m
 		}[m
 [m
 		// Copy data to texture via immediate mode submit[m
[31m-		ImmediateSubmit([&](VkCommandBuffer cmd) {[m
[32m+[m		[32mImmediateSubmit([&](VkCommandBuffer cmd)[m
[32m+[m						[32m{[m
 			// ~ Transition to transfer optimal[m
 			VkImageSubresourceRange range = {};[m
 			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;[m
[36m@@ -176,35 +207,36 @@[m [mnamespace yoyo[m
 			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_to_shader_barrier); });[m
 [m
 		// Destroy staging buffer[m
[31m-		vmaDestroyBuffer(m_allocator, staging_buffer.buffer, staging_buffer.allocation);[m
[32m+[m		[32mvmaDestroyBuffer(s_allocator, staging_buffer.buffer, staging_buffer.allocation);[m
 [m
 		if (!texture->IsInitialized())[m
 		{[m
 			// Create image view[m
 [m
 			VkImageViewType view_type = {};[m
[31m-			switch(texture->GetTextureType())[m
[32m+[m			[32mswitch (texture->GetTextureType())[m
[32m+[m			[32m{[m
[32m+[m			[32mcase (TextureType::Array):[m
 			{[m
[31m-				case(TextureType::Array):[m
[31m-				{[m
[31m-					view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;[m
[31m-				}break;[m
[31m-				default:[m
[31m-					view_type = VK_IMAGE_VIEW_TYPE_2D;[m
[31m-					break;[m
[32m+[m				[32mview_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;[m
[32m+[m			[32m}[m
[32m+[m			[32mbreak;[m
[32m+[m			[32mdefault:[m
[32m+[m				[32mview_type = VK_IMAGE_VIEW_TYPE_2D;[m
[32m+[m				[32mbreak;[m
 			}[m
 [m
 			VkImageViewCreateInfo image_view_info = vkinit::ImageViewCreateInfo(texture->allocated_image.image, ConvertTextureFormat(texture->format), VK_IMAGE_ASPECT_COLOR_BIT, texture->layers, view_type);[m
[31m-			VK_CHECK(vkCreateImageView(m_device, &image_view_info, nullptr, &texture->image_view));[m
[32m+[m			[32mVK_CHECK(vkCreateImageView(s_device, &image_view_info, nullptr, &texture->image_view));[m
 [m
 			// Create Sampler[m
 			VkSamplerCreateInfo sampler_info = vkinit::SamplerCreateInfo((VkFilter)texture->GetSamplerType(), (VkSamplerAddressMode)texture->GetAddressMode());[m
[31m-			VK_CHECK(vkCreateSampler(m_device, &sampler_info, nullptr, &texture->sampler));[m
[32m+[m			[32mVK_CHECK(vkCreateSampler(s_device, &sampler_info, nullptr, &texture->sampler));[m
 [m
[31m-			m_deletion_queue->Push([=]()[m
[32m+[m			[32ms_deletion_queue->Push([=]()[m
 								   {[m
[31m-				vkDestroySampler(m_device, texture->sampler, nullptr);[m
[31m-				vkDestroyImageView(m_device, texture->image_view, nullptr); });[m
[32m+[m				[32mvkDestroySampler(s_device, texture->sampler, nullptr);[m
[32m+[m				[32mvkDestroyImageView(s_device, texture->image_view, nullptr); });[m
 		}[m
 [m
 		return true;[m
[36m@@ -212,12 +244,12 @@[m [mnamespace yoyo[m
 [m
 	void VulkanResourceManager::MapMemory(VmaAllocation allocation, void **data)[m
 	{[m
[31m-		vmaMapMemory(m_allocator, allocation, data);[m
[32m+[m		[32mvmaMapMemory(s_allocator, allocation, data);[m
 	}[m
 [m
 	void VulkanResourceManager::UnmapMemory(VmaAllocation allocation)[m
 	{[m
[31m-		vmaUnmapMemory(m_allocator, allocation);[m
[32m+[m		[32mvmaUnmapMemory(s_allocator, allocation);[m
 	}[m
 [m
 	Ref<VulkanShaderModule> VulkanResourceManager::CreateShaderModule(const std::string &shader_path)[m
[36m@@ -235,14 +267,14 @@[m [mnamespace yoyo[m
 			shader_info.pCode = reinterpret_cast<uint32_t *>(buffer);[m
 			shader_info.codeSize = buffer_size;[m
 [m
[31m-			VK_CHECK(vkCreateShaderModule(m_device, &shader_info, nullptr, &shader_module->module));[m
[32m+[m			[32mVK_CHECK(vkCreateShaderModule(s_device, &shader_info, nullptr, &shader_module->module));[m
 [m
 			shader_module->code.resize(buffer_size / sizeof(uint32_t));[m
 			memcpy(shader_module->code.data(), buffer, buffer_size);[m
 			delete buffer;[m
 [m
[31m-			m_deletion_queue->Push([=]()[m
[31m-								   { vkDestroyShaderModule(m_device, shader_module->module, nullptr); });[m
[32m+[m			[32ms_deletion_queue->Push([=]()[m
[32m+[m								[32m   { vkDestroyShaderModule(s_device, shader_module->module, nullptr); });[m
 		}[m
 		else[m
 		{[m
[36m@@ -261,12 +293,12 @@[m [mnamespace yoyo[m
 		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;[m
 		alloc_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;[m
 [m
[31m-		VK_CHECK(vmaCreateImage(m_allocator, &image_info, &alloc_info, &image.image, &image.allocation, nullptr));[m
[32m+[m		[32mVK_CHECK(vmaCreateImage(s_allocator, &image_info, &alloc_info, &image.image, &image.allocation, nullptr));[m
 [m
 		if (manage_memory)[m
 		{[m
[31m-			m_deletion_queue->Push([=]()[m
[31m-								   { vmaDestroyImage(m_allocator, image.image, image.allocation); });[m
[32m+[m			[32ms_deletion_queue->Push([=]()[m
[32m+[m								[32m   { vmaDestroyImage(s_allocator, image.image, image.allocation); });[m
 		}[m
 [m
 		return image;[m
[36m@@ -274,7 +306,7 @@[m [mnamespace yoyo[m
 [m
 	const size_t VulkanResourceManager::PadToUniformBufferSize(size_t original_size)[m
 	{[m
[31m-		size_t min_ubo_allignment = m_physical_device_props.limits.minUniformBufferOffsetAlignment;[m
[32m+[m		[32msize_t min_ubo_allignment = s_physical_device_props.limits.minUniformBufferOffsetAlignment;[m
 		size_t aligned_size = original_size;[m
 [m
 		if (min_ubo_allignment > 0)[m
[36m@@ -286,7 +318,7 @@[m [mnamespace yoyo[m
 [m
 	const size_t VulkanResourceManager::PadToStorageBufferSize(size_t original_size)[m
 	{[m
[31m-		size_t min_ubo_allignment = m_physical_device_props.limits.minStorageBufferOffsetAlignment;[m
[32m+[m		[32msize_t min_ubo_allignment = s_physical_device_props.limits.minStorageBufferOffsetAlignment;[m
 		size_t aligned_size = original_size;[m
 [m
 		if (min_ubo_allignment > 0)[m
[36m@@ -298,7 +330,7 @@[m [mnamespace yoyo[m
 [m
 	void VulkanResourceManager::ImmediateSubmit(std::function<void(VkCommandBuffer)> &&fn)[m
 	{[m
[31m-		VkCommandBuffer cmd = m_upload_context.command_buffer;[m
[32m+[m		[32mVkCommandBuffer cmd = s_upload_context.command_buffer;[m
 [m
 		VkCommandBufferBeginInfo begin_info = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);[m
 		vkBeginCommandBuffer(cmd, &begin_info);[m
[36m@@ -313,18 +345,18 @@[m [mnamespace yoyo[m
 		submit_info.pNext = nullptr;[m
 [m
 		submit_info.commandBufferCount = 1;[m
[31m-		submit_info.pCommandBuffers = &m_upload_context.command_buffer;[m
[31m-		vkQueueSubmit(m_queues->graphics.queue, 1, &submit_info, m_upload_context.fence);[m
[32m+[m		[32msubmit_info.pCommandBuffers = &s_upload_context.command_buffer;[m
[32m+[m		[32mvkQueueSubmit(s_queues->graphics.queue, 1, &submit_info, s_upload_context.fence);[m
 [m
[31m-		vkWaitForFences(m_device, 1, &m_upload_context.fence, VK_TRUE, 1000000000);[m
[31m-		vkResetFences(m_device, 1, &m_upload_context.fence);[m
[32m+[m		[32mvkWaitForFences(s_device, 1, &s_upload_context.fence, VK_TRUE, 1000000000);[m
[32m+[m		[32mvkResetFences(s_device, 1, &s_upload_context.fence);[m
 [m
 		// Reset command pool[m
[31m-		vkResetCommandPool(m_device, m_upload_context.command_pool, 0);[m
[32m+[m		[32mvkResetCommandPool(s_device, s_upload_context.command_pool, 0);[m
 	}[m
 [m
 	DescriptorBuilder VulkanResourceManager::AllocateDescriptor()[m
 	{[m
[31m-		return DescriptorBuilder::Begin(m_descriptor_layout_cache.get(), m_descriptor_allocator.get());[m
[32m+[m		[32mreturn DescriptorBuilder::Begin(s_descriptor_layout_cache.get(), s_descriptor_allocator.get());[m
 	}[m
 }[m
\ No newline at end of file[m
[1mdiff --git a/src/Renderer/Vulkan/VulkanResourceManager.h b/src/Renderer/Vulkan/VulkanResourceManager.h[m
[1mindex 4419b7f..778058d 100644[m
[1m--- a/src/Renderer/Vulkan/VulkanResourceManager.h[m
[1m+++ b/src/Renderer/Vulkan/VulkanResourceManager.h[m
[36m@@ -8,7 +8,11 @@[m
 [m
 namespace yoyo[m
 {[m
[31m-    const int MAX_MESH_VERTICES = 100000;[m
[32m+[m[32m    constexpr int MAX_MESH_VERTICES = 150080;[m
[32m+[m[32m    constexpr size_t STAGING_BUFFER_SIZE = sizeof(Vertex) * MAX_MESH_VERTICES;[m
[32m+[m
[32m+[m[32m    template <typename VertexType, typename IndexType>[m
[32m+[m[32m    class VulkanMesh;[m
 [m
     class VulkanTexture;[m
     class VulkanRenderer;[m
[36m@@ -17,62 +21,64 @@[m [mnamespace yoyo[m
     class YAPI VulkanResourceManager[m
     {[m
     public:[m
[31m-        static void Init(VulkanRenderer* renderer);[m
[32m+[m[32m        static void Init(VulkanRenderer *renderer);[m
         static void Shutdown();[m
 [m
[31m-        template<typename MeshType>[m
[31m-        static bool UploadMesh(MeshType* mesh)[m
[32m+[m[32m        template <typename VertexType, typename IndexType>[m
[32m+[m[32m        static bool UploadMesh(VulkanMesh<VertexType, IndexType>* mesh)[m
         {[m
             YASSERT(mesh, "Invalid mesh!");[m
             YASSERT(mesh->GetVertexCount() > 0, "Empty mesh!");[m
[31m-            YASSERT(mesh->GetVertexCount() * mesh->GetVertexSize() <= MAX_MESH_VERTICES * mesh->GetVertexSize(), "Mesh exceeds max vertex buffer size!");[m
 [m
[31m-            mesh->vertex_buffer = CreateBuffer<>(mesh->GetVertexCount() * mesh->GetVertexSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);[m
[32m+[m[32m            size_t vertex_data_size = mesh->GetVertexCount() * mesh->GetVertexSize();[m
[32m+[m[32m            YASSERT(vertex_data_size <= STAGING_BUFFER_SIZE, "Mesh exceeds max vertex buffer size!");[m
[32m+[m
[32m+[m[32m            mesh->vertex_buffer = CreateBuffer<>(vertex_data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);[m
 [m
[31m-            void* data;[m
[31m-            MapMemory(m_mesh_staging_buffer.allocation, &data);[m
[32m+[m[32m            void *data;[m
[32m+[m[32m            MapMemory(StagingBuffer().allocation, &data);[m
             memcpy(data, mesh->GetVertices().data(), mesh->GetVertexCount() * mesh->GetVertexSize());[m
[31m-            UnmapMemory(m_mesh_staging_buffer.allocation);[m
[32m+[m[32m            UnmapMemory(StagingBuffer().allocation);[m
 [m
[31m-            ImmediateSubmit([&](VkCommandBuffer cmd) {[m
[32m+[m[32m            ImmediateSubmit([&](VkCommandBuffer cmd)[m
[32m+[m[32m                            {[m
                 VkBufferCopy region = {};[m
                 region.dstOffset = 0;[m
                 region.srcOffset = 0;[m
                 region.size = mesh->GetVertexCount() * mesh->GetVertexSize();[m
 [m
[31m-                vkCmdCopyBuffer(cmd, m_mesh_staging_buffer.buffer, mesh->vertex_buffer.buffer, 1, &region);[m
[31m-            });[m
[32m+[m[32m                vkCmdCopyBuffer(cmd, StagingBuffer().buffer, mesh->vertex_buffer.buffer, 1, &region); });[m
 [m
             if (!mesh->GetIndices().empty())[m
             {[m
                 mesh->index_buffer = CreateBuffer<uint32_t>(mesh->GetIndexCount() * mesh->GetIndexSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);[m
 [m
[31m-                void* data;[m
[31m-                vmaMapMemory(m_allocator, m_mesh_staging_buffer.allocation, &data);[m
[32m+[m[32m                void *data;[m
[32m+[m[32m                vmaMapMemory(Allocator(), StagingBuffer().allocation, &data);[m
                 memcpy(data, mesh->GetIndices().data(), mesh->GetIndexCount() * mesh->GetIndexSize());[m
[31m-                vmaUnmapMemory(m_allocator, m_mesh_staging_buffer.allocation);[m
[32m+[m[32m                vmaUnmapMemory(Allocator(), StagingBuffer().allocation);[m
 [m
[31m-                ImmediateSubmit([&](VkCommandBuffer cmd) {[m
[32m+[m[32m                ImmediateSubmit([&](VkCommandBuffer cmd)[m
[32m+[m[32m                                {[m
                     VkBufferCopy region = {};[m
                     region.dstOffset = 0;[m
                     region.srcOffset = 0;[m
                     region.size = mesh->GetIndexCount() * mesh->GetIndexSize();[m
 [m
[31m-                    vkCmdCopyBuffer(cmd, m_mesh_staging_buffer.buffer, mesh->index_buffer.buffer, 1, &region);[m
[31m-                });[m
[32m+[m[32m                    vkCmdCopyBuffer(cmd, StagingBuffer().buffer, mesh->index_buffer.buffer, 1, &region); });[m
             }[m
 [m
             return true;[m
         }[m
 [m
[31m-        static bool UploadTexture(VulkanTexture* texture);[m
[32m+[m[32m        static bool UploadTexture(VulkanTexture *texture);[m
 [m
[31m-        static void MapMemory(VmaAllocation allocation, void** data);[m
[32m+[m[32m        static void MapMemory(VmaAllocation allocation, void **data);[m
         static void UnmapMemory(VmaAllocation allocation);[m
 [m
[31m-        static Ref<VulkanShaderModule> CreateShaderModule(const std::string& shader_path);[m
[32m+[m[32m        static Ref<VulkanShaderModule> CreateShaderModule(const std::string &shader_path);[m
 [m
[31m-        template<typename T = void>[m
[32m+[m[32m        template <typename T = void>[m
         static AllocatedBuffer<T> CreateBuffer(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VkMemoryPropertyFlags memory_props = 0, bool manage_memory = true /*If true memory will be managed by resource manager */)[m
         {[m
             VkBufferCreateInfo buffer_info = {};[m
[36m@@ -87,14 +93,13 @@[m [mnamespace yoyo[m
             alloc_info.usage = memory_usage;[m
             alloc_info.requiredFlags = memory_props;[m
 [m
[31m-            AllocatedBuffer<T> buffer;[m
[31m-            VK_CHECK(vmaCreateBuffer(m_allocator, &buffer_info, &alloc_info, &buffer.buffer, &buffer.allocation, nullptr));[m
[32m+[m[32m            AllocatedBuffer<T> buffer = {};[m
[32m+[m[32m            VK_CHECK(vmaCreateBuffer(Allocator(), &buffer_info, &alloc_info, &buffer.buffer, &buffer.allocation, nullptr));[m
 [m
             if (manage_memory)[m
             {[m
[31m-                m_deletion_queue->Push([=]() {[m
[31m-                    vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);[m
[31m-                });[m
[32m+[m[32m                DeletionQueue().Push([=]()[m
[32m+[m[32m                                       { vmaDestroyBuffer(Allocator(), buffer.buffer, buffer.allocation); });[m
             }[m
 [m
             return buffer;[m
[36m@@ -105,26 +110,15 @@[m [mnamespace yoyo[m
         static const size_t PadToUniformBufferSize(size_t original_size);[m
         static const size_t PadToStorageBufferSize(size_t original_size);[m
 [m
[31m-        static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& fn);[m
[32m+[m[32m        static void ImmediateSubmit(std::function<void(VkCommandBuffer)> &&fn);[m
 [m
         static DescriptorBuilder AllocateDescriptor();[m
     private:[m
         VulkanResourceManager() = default;[m
[31m-        ~VulkanResourceManager() {};[m
[31m-    private:[m
[31m-        inline static AllocatedBuffer<void> m_mesh_staging_buffer; // General purpose staging buffer for mesh uploads[m
[31m-        inline static VulkanUploadContext m_upload_context;[m
[31m-[m
[31m-        inline static VkDevice m_device;[m
[31m-        inline static VkPhysicalDeviceProperties m_physical_device_props;[m
[31m-        inline static VkPhysicalDevice m_physical_device;[m
[31m-        inline static VkInstance m_instance;[m
[31m-[m
[31m-        inline static VulkanDeletionQueue* m_deletion_queue;[m
[31m-        inline static VulkanQueues* m_queues;[m
[32m+[m[32m        ~VulkanResourceManager(){};[m
 [m
[31m-        inline static VmaAllocator m_allocator;[m
[31m-        inline static Ref<DescriptorAllocator> m_descriptor_allocator;[m
[31m-        inline static Ref<DescriptorLayoutCache> m_descriptor_layout_cache;[m
[32m+[m[32m        static AllocatedBuffer<> &StagingBuffer();[m
[32m+[m[32m        const static VmaAllocator Allocator();[m
[32m+[m[32m        static VulkanDeletionQueue& DeletionQueue();[m
     };[m
 }[m
\ No newline at end of file[m
[1mdiff --git a/src/Resource/Resource.h b/src/Resource/Resource.h[m
[1mindex 1e377d9..892f92a 100644[m
[1m--- a/src/Resource/Resource.h[m
[1m+++ b/src/Resource/Resource.h[m
[36m@@ -14,14 +14,13 @@[m [mnamespace yoyo[m
     // ex. some_folder/my_file.extension => my_file.extension[m
     const std::string FileNameFromFullPath(const std::string& full_path);[m
 [m
[31m-    // TODO: Change from string to const char*[m
     #define RESOURCE_TYPE(type) const ResourceType Type() const override {return s_resource_type;}\[m
                                 inline static const ResourceType s_resource_type = Platform::GenerateUUIDV4();\[m
                                 inline static const std::string s_resource_type_name = #type;\[m
                                 inline static const std::string TypeName() {return s_resource_type_name;}\[m
 [m
     // Base class for all resources in the engine[m
[31m-    class Resource[m
[32m+[m[32m    class YAPI Resource[m
     {[m
     public:[m
         Resource() = default;[m
[1mdiff --git a/src/Resource/ResourceEvent.h b/src/Resource/ResourceEvent.h[m
[1mindex 974d204..4d0c617 100644[m
[1m--- a/src/Resource/ResourceEvent.h[m
[1m+++ b/src/Resource/ResourceEvent.h[m
[36m@@ -6,7 +6,7 @@[m
 namespace yoyo[m
 {[m
     template<typename MeshType>[m
[31m-    class MeshCreatedEvent : public Event[m
[32m+[m[32m    class YAPI MeshCreatedEvent : public Event[m
     {[m
     public:[m
         EVENT_TYPE(MeshCreatedEvent)[m
[1mdiff --git a/vendor/Hurno b/vendor/Hurno[m
[1m--- a/vendor/Hurno[m
[1m+++ b/vendor/Hurno[m
[36m@@ -1 +1 @@[m
[31m-Subproject commit 282c586af8099e0a5018d96a5c98775744d48dce[m
[32m+[m[32mSubproject commit 282c586af8099e0a5018d96a5c98775744d48dce-dirty[m
