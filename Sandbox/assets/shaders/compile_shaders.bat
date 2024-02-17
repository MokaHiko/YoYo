cd /D "%~dp0"
"%VULKAN_SDK%/bin/glslc.exe" lit_shader.vert -o lit_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" lit_shader.frag -o lit_shader.frag.spv

"%VULKAN_SDK%/bin/glslc.exe" offscreen_shadow_shader.vert -o offscreen_shadow_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" offscreen_shadow_shader.frag -o offscreen_shadow_shader.frag.spv

"%VULKAN_SDK%/bin/glslc.exe" full_screen_shader.vert -o full_screen_shader.vert.spv
"%VULKAN_SDK%/bin/glslc.exe" blit_shader.frag -o blit_shader.frag.spv