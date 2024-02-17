#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 v_position_world_space;
layout(location = 1) out vec3 v_color;
layout(location = 2) out vec3 v_normal_world_space;
layout(location = 3) out vec2 v_uv;

struct ObjectData 
{
	mat4 model_matrix;
};

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

layout(std140, set = 0, binding = 5) readonly buffer objectData{
  ObjectData objects[];
};

void main()
{
	mat4 model_matrix = objects[gl_BaseInstance].model_matrix;

	v_position_world_space = vec3(model_matrix * vec4(position, 1.0f)); 
	v_color = color;

	v_uv = uv;

	// v_normal_world_space = normalize(mat3(model_matrix) * normal); // For uniform scaled objects
	v_normal_world_space = normalize(mat3(transpose(inverse(model_matrix))) * normal); 

  // // TODO: Specifiy which dir light pass
	gl_Position = dir_lights[0].view_proj * vec4(v_position_world_space, 1.0f);
}