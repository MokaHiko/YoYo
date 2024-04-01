#version 460

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 v_position_world_space;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec3 v_normal_world_space;
layout(location = 3) in vec2 v_uv;

layout(location = 4) in vec4 v_position_light_space;
layout(location = 5) in float v_focused_bone_weight;

struct DirectionalLight {
  mat4 view_proj;

  vec4 color;
  vec4 direction;
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

layout(set = 0, binding = 4) uniform sampler2D shadow_map;

// Descriptor set 1 is reserved for texture information
layout(set = 1, binding = 0) uniform sampler2D main_texture;
layout(set = 1, binding = 1) uniform sampler2D specular_texture;

// Descriptor set 2 is reserved for public material properties
layout(set = 2, binding = 0) uniform Material {
  vec4 diffuse_color;
  vec4 specular_color;

  int focused_bone_index;
};

vec4 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
float CalculateShadows(vec4 frag_position_light_space, vec3 normal, vec3 light_dir);

void main() {
  // vec3 normal = normalize(v_normal_world_space);
  // vec3 view_dir = normalize(v_position_world_space);

  // vec3 ambient = vec3(0.15f);
  // vec4 final_color = vec4(0.0f);

  // // Lights
  // for (int i = 0; i < dir_light_count; i++) {
  //   final_color += CalculateDirectionalLight(dir_lights[i], normal, view_dir);
  // }

  // // Shadow
  // float shadow = (1 - CalculateShadows(v_position_light_space, normal, -dir_lights[0].direction.xyz));

  // frag_color = vec4(ambient, 0.0f) + (shadow * final_color); 
  vec4 white = vec4(1.0f, 1.0f, 1.0f, 0.25f);
  vec4 cyan = vec4(0.0f, 1.0f, 1.0f, 1.0f);
  vec4 color = mix(white, cyan, v_focused_bone_weight);
  frag_color = color;
  // frag_color = vec4(0.0f, 1.0f, 1.0f, v_focused_bone_weight);
}

vec4 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir) {
  vec3 light_dir = normalize(-light.direction.xyz);

  float diff_factor = max(dot(normal, light_dir), 0.25f);
  vec4 diffuse = diff_factor * light.color;
  diffuse *= texture(main_texture, v_uv) * diffuse_color;

  vec3 reflect_dir = reflect(light_dir.xyz, normal);
	float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.5f), 32);
  vec4 specular = specular_factor * light.color * texture(specular_texture, v_uv);

  return vec4(diffuse.xyz + specular.xyz, 1.0f);
}

float CalculateShadows(vec4 frag_position_light_space, vec3 normal, vec3 light_dir)
{
  // Manual perspective divide (normally done after vertex shader)
  vec3 proj_coords = frag_position_light_space.xyz / frag_position_light_space.w;

  // Normalize only uv to [0,1]
  proj_coords.xy = proj_coords.xy * 0.5f + 0.5f;

  float sampled_depth = texture(shadow_map, proj_coords.xy).r;
  float current_depth = proj_coords.z;

  float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);  
  float shadow = current_depth - bias > sampled_depth ? 1.0f : 0.0f;

  return shadow;
}