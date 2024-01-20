#version 460

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec2 v_uv;

layout(set = 0, binding = 0) uniform SceneData {
  mat4 view;
  mat4 proj;
};

// Descriptor set 1 is reserved for texture information
layout(set = 1, binding = 0) uniform sampler2D main_texture;

// Descriptor set 2 is reserved for public material properties
layout(set = 2, binding = 0) uniform Material { 
  vec4 diffuse_color; 
  vec4 specular_color; 
};

void main() {
  vec4 final_color = {1.0, 1.0, 1.0, 1.0f};
  final_color *= diffuse_color;
  final_color *= texture(main_texture, v_uv);
  frag_color = final_color;
}