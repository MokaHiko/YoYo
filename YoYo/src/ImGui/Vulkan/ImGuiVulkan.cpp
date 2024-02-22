#include "ImGuiVulkan.h"

#include "Events/Event.h"

#include "ImGui/ImGuiLayer.h"

#include <imgui_impl_vulkan.h>

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "Core/Application.h"
#include "Renderer/RendererLayer.h"

#include "Renderer/Vulkan/VulkanRenderer.h"

namespace yoyo
{
	static VulkanRenderer* s_renderer;

	void ImGuiLayer::OnAttach()
	{
	}

	void ImGuiLayer::OnDetatch()
	{
		if(IsEnabled())
		{
			OnDisable();
		}
	}

	void ImGuiLayer::OnEnable()
	{
		m_window = (SDL_Window*)Platform::NativeAppWindow();
		s_renderer = static_cast<VulkanRenderer*>(m_app->FindLayer<RendererLayer>()->NativeRenderer());

		// 1: create descriptor pool for IMGUI
		//  the size of the pool is very oversize, but it's copied from imgui demo
		//  itself.
		VkDescriptorPoolSize pool_sizes[] = {
			{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		VK_CHECK(vkCreateDescriptorPool(s_renderer->Device(), &pool_info, nullptr, &imguiPool));

		// 2: initialize imgui library

		// this initializes the core structures of imgui
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		io.ConfigWindowsResizeFromEdges = true;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// this initializes imgui for SDL
		ImGui_ImplSDL2_InitForVulkan(m_window);

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = s_renderer->Instance();
		init_info.PhysicalDevice = s_renderer->PhysicalDevice();

		init_info.Device = s_renderer->Device();
		init_info.Queue = s_renderer->Queues().graphics.queue;
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.RenderPass = s_renderer->SwapChainRenderPass();
		ImGui_ImplVulkan_Init(&init_info);

		SetupImGuiStyle(1, 1.0f);

		// execute a gpu command to upload imgui font textures
		// VulkanResourceManager::ImmediateSubmit([&](VkCommandBuffer cmd){ 
		// 		ImGui_ImplVulkan_CreateFontsTexture(cmd); 
		// });

		// clear font textures from cpu data
		// ImGui_ImplVulkan_DestroyFontUploadObjects();

		// add the destroy the imgui created structures
		s_renderer->DeletionQueue().Push([=]() {
			vkDestroyDescriptorPool(s_renderer->Device(), imguiPool, nullptr);
		});
	}

	void ImGuiLayer::OnDisable()
	{
		vkDeviceWaitIdle(s_renderer->Device());

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	bool ImGuiLayer::OnEvent(Ref<Event> event)
	{
		return false;
	}

	void ImGuiLayer::OnMainPassBegin(void* render_context)
	{
		VkCommandBuffer cmd = static_cast<VulkanRenderContext*>(render_context)->cmd;
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		static bool show_demo_window = true;
		ImGui::ShowDemoWindow(&show_demo_window);

		//ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::OnMainPassEnd(void* render_context)
	{
		VkCommandBuffer cmd = static_cast<VulkanRenderContext*>(render_context)->cmd;

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	}
}