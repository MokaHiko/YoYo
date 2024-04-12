cd /D "%~dp0"
"%VULKAN_SDK%/bin/glslc.exe" lit_shader.vert -o lit_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" lit_skinned_shader.vert -o lit_skinned_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" lit_instanced_shader.vert -o lit_instanced_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" lit_shader.frag -o lit_shader.frag.spv

"%VULKAN_SDK%/bin/glslc.exe" offscreen_shadow_shader.vert -o offscreen_shadow_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" offscreen_shadow_instanced_shader.vert -o offscreen_shadow_instanced_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" offscreen_shadow_shader.frag -o offscreen_shadow_shader.frag.spv
"%VULKAN_SDK%/bin/glslc.exe" offscreen_skinned_shadow_shader.vert -o offscreen_skinned_shadow_shader.vert.spv

"%VULKAN_SDK%/bin/glslc.exe" unlit_instanced_shader.vert -o unlit_instanced_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" unlit_particle_instanced_shader.vert -o unlit_particle_instanced_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" unlit_particle_shader.frag -o unlit_particle_shader.frag.spv

"%VULKAN_SDK%/bin/glslc.exe" unlit_shader.frag -o unlit_shader.frag.spv

"%VULKAN_SDK%/bin/glslc.exe" full_screen_shader.vert -o full_screen_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" blit_shader.frag -o blit_shader.frag.spv

@REM Debug Shaders
"%VULKAN_SDK%/bin/glslc.exe" lit_skinned_debug_shader.vert -o lit_skinned_debug_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" lit_skinned_debug_shader.frag -o lit_skinned_debug_shader.frag.spv

"%VULKAN_SDK%/bin/glslc.exe" unlit_collider_debug_shader.vert -o unlit_collider_debug_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" unlit_collider_debug_shader.frag -o unlit_collider_debug_shader.frag.spv