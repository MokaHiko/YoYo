cmake_minimum_required(VERSION 3.0.0)
project(YoYo VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

option(USE_SIMD "Use SIMD math" OFF)

add_library(YoYo SHARED
    src/Defines.h
    src/Defines.cpp
    src/Platform/Platform.h

    src/Math/Math.h
    src/Math/Math.cpp
    src/Math/MathTypes.h
    src/Math/MathTypes.cpp
    src/Math/Quaternion.h
    src/Math/Quaternion.cpp
    src/Math/MatrixTransform.h
    src/Math/MatrixTransform.cpp
    src/Math/Random.h
    src/Math/Random.cpp

    src/Core/Assert.h
    src/Core/Assert.cpp
    src/Core/Memory.h
    src/Core/Memory.cpp
    src/Core/Time.h
    src/Core/Time.cpp
    src/Core/Log.h
    src/Core/Log.cpp
    src/Core/Layer.h
    src/Core/Layer.cpp
    src/Core/Application.h
    src/Core/Application.cpp

    src/Resource/Resource.h
    src/Resource/Resource.cpp
    src/Resource/ResourceEvent.h
    src/Resource/ResourceEvent.cpp
    src/Resource/ResourceManager.h
    src/Resource/ResourceManager.cpp

    src/Events/Event.h
    src/Events/Event.cpp
    src/Events/ApplicationEvent.h
    src/Events/ApplicationEvent.cpp

    src/Input/Input.h
    src/Input/Input.cpp
    src/Input/InputEvent.h
    src/Input/InputEvent.cpp

    src/ImGui/ImGuiLayer.h
    src/ImGui/ImGuiLayer.cpp

    src/Renderer/Renderer.h
    src/Renderer/Renderer.cpp
    src/Renderer/RenderPass.h
    src/Renderer/RenderPass.cpp
    src/Renderer/Shader.h
    src/Renderer/Shader.cpp
    src/Renderer/Material.h
    src/Renderer/Material.cpp
    src/Renderer/Mesh.h
    src/Renderer/Mesh.cpp
    src/Renderer/Texture.h
    src/Renderer/Texture.cpp
    src/Renderer/RendererLayer.h
    src/Renderer/RendererLayer.cpp

    src/Renderer/Model.h
    src/Renderer/Model.cpp

    src/Renderer/Camera.h
    src/Renderer/Camera.cpp
    src/Renderer/Light.h
    src/Renderer/Light.cpp
    src/Renderer/RenderScene.h
    src/Renderer/RenderScene.cpp

    src/Core/EntryPoint.h
    include/Yoyo.h
)
target_precompile_headers(${PROJECT_NAME} PUBLIC src/ypch.h)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PUBLIC src include)

add_compile_definitions(Y_EXPORT)

add_subdirectory(vendor/Hurno)

find_package(Vulkan REQUIRED)
file(GLOB IMGUI_SOURCES "vendor/ImGui/*.cpp" "vendor/ImGui/backends/imgui_impl_vulkan.cpp" "vendor/ImGui/backends/imgui_impl_sdl2.cpp")
file(GLOB IMGUI_HEADERS "vendor/ImGui/*.h" "vendor/ImGui/backends/imgui_impl_vulkan.h" "vendor/ImGui/backends/imgui_impl_sdl2.h")
add_library(imgui STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC vendor/ImGui vendor/ImGui/backends)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL2::SDL2)

target_link_libraries(${PROJECT_NAME} PUBLIC Hurno imgui)

# TODO Detect build type
if(true)
    add_compile_definitions(Y_DEBUG)
endif()

if(MSVC)
    add_compile_definitions(Y_COMPILER_MSVC)
endif()

if (WIN32)
    add_compile_definitions(Y_PLATFORM_WIN32)
    target_sources(${PROJECT_NAME} PRIVATE src/Platform/Win32/PlatformWin32.h src/Platform/Win32/PlatformWin32.cpp)

    add_subdirectory(vendor/SDL)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 vulkan-1)

    target_include_directories(${PROJECT_NAME} PRIVATE vendor/SDL/include $ENV{VULKAN_SDK}/include)
    target_link_directories(${PROJECT_NAME} PRIVATE $ENV{VULKAN_SDK}/Lib)

    set(YRENDERERTYPE "VULKAN")
elseif (APPLE)
    add_compile_definitions(Y_PLATFORM_APPLE)
    target_sources(${PROJECT_NAME} PRIVATE src/Platform/OSX/OSX.h src/Platform/OSX/OSX.cpp)

    add_subdirectory(vendor/SDL)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 vulkan)
    target_include_directories(${PROJECT_NAME} PRIVATE vendor/SDL/include "/Users/caltax/VulkanSDK/1.3.243.0/macOS/include")

    target_link_directories(${PROJECT_NAME} PRIVATE "/Users/caltax/VulkanSDK/1.3.243.0/macOS/lib")
    set(YRENDERERTYPE "VULKAN")
endif()

if (YRENDERERTYPE STREQUAL "VULKAN")
    add_subdirectory(vendor/VkBootstrap)
    add_subdirectory(vendor/Vma)
    
    set(SPIRV_REFLECT_EXECUTABLE OFF)
    set(SPIRV_REFLECT_STATIC_LIB ON)
    add_subdirectory(vendor/SPIRVReflect)

    # TODO: Make spriv reflect static fork for static lib
    target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::VulkanMemoryAllocator vk-bootstrap spirv-reflect-static)
    target_sources(${PROJECT_NAME} PRIVATE 
        src/Renderer/Vulkan/VulkanStructures.cpp
        src/Renderer/Vulkan/VulkanStructures.h
        src/Renderer/Vulkan/VulkanUtils.h
        src/Renderer/Vulkan/VulkanUtils.cpp
        src/Renderer/Vulkan/VulkanInitializers.h
        src/Renderer/Vulkan/VulkanInitializers.cpp

        src/Renderer/Vulkan/VulkanTexture.h
        src/Renderer/Vulkan/VulkanTexture.cpp
        src/Renderer/Vulkan/VulkanShader.h
        src/Renderer/Vulkan/VulkanShader.cpp
        src/Renderer/Vulkan/VulkanMesh.h
        src/Renderer/Vulkan/VulkanMesh.cpp
        src/Renderer/Vulkan/VulkanMaterial.h
        src/Renderer/Vulkan/VulkanMaterial.cpp

        src/Renderer/Vulkan/VulkanMaterialSystem.h
        src/Renderer/Vulkan/VulkanMaterialSystem.cpp
        src/Renderer/Vulkan/VulkanResourceManager.h
        src/Renderer/Vulkan/VulkanResourceManager.cpp

        src/Renderer/Vulkan/VulkanScene.h
        src/Renderer/Vulkan/VulkanScene.cpp

        src/Renderer/Vulkan/VulkanRenderer.h
        src/Renderer/Vulkan/VulkanRenderer.cpp

        src/ImGui/Vulkan/ImGuiVulkan.h
        src/ImGui/Vulkan/ImGuiVulkan.cpp
    )
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
