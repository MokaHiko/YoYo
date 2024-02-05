#version 460

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 v_position_world_space;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec3 v_normal_world_space;
layout(location = 3) in vec2 v_uv;

struct DirectionalLight {
  vec4 color;
  vec3 direction;
};

layout(set = 0, binding = 0) uniform SceneData {
  mat4 view;
  mat4 proj;

  uint dir_light_count;
  uint point_light_count;
  uint spot_light_count;
  uint area_light_count;
};

layout(std140, set = 0, binding = 1) readonly buffer DirectionalLights {
  DirectionalLight dir_lights[];
};

// Descriptor set 1 is reserved for texture information
layout(set = 1, binding = 0) uniform sampler2D main_texture;

// Descriptor set 2 is reserved for public material properties
layout(set = 2, binding = 0) uniform Material {
  vec4 diffuse_color;
  vec4 specular_color;
};

vec4 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir) {
  vec3 light_dir = normalize(-light.direction);

  float diff_factor = max(dot(normal, light_dir), 0.25f);

  vec4 diffuse = diff_factor * light.color;
  diffuse *= texture(main_texture, v_uv) * diffuse_color;

  return diffuse;
}

void main() {
  vec3 normal = normalize(v_normal_world_space);
  vec3 view_dir = normalize(v_position_world_space);

  vec4 final_color = vec4(0.0f);
  for (int i = 0; i < dir_light_count; i++) {
    final_color += CalculateDirectionalLight(dir_lights[i], normal, view_dir);
  }

  frag_color = final_color;
}