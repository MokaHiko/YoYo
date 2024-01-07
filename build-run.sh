# Compile shaders
echo compiling shaders...
"/Users/caltax/VulkanSDK/1.3.243.0/macOS/bin/glslc" assets/shaders/lit_shader.vert -o assets/shaders/lit_shader.vert.spv
"/Users/caltax/VulkanSDK/1.3.243.0/macOS/bin/glslc" assets/shaders/lit_shader.frag -o assets/shaders/lit_shader.frag.spv

# Configure cmake project
cmake -S ./ -B ./build

# Building
make -C ./build

# Run file
./build/Sandbox/Sandbox