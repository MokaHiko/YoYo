#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 v_position_world_space;
layout(location = 1) out vec3 v_color;
layout(location = 2) out vec3 v_normal_world_space;
layout(location = 3) out vec2 v_uv;

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 proj;
};

void main()
{
	mat4 model_matrix = mat4(1.0f);

	v_position_world_space = mat3(model_matrix) * position; 
	v_color = color;
	v_uv = uv;

	//v_normal_world_space = normalize(mat3(model_matrix) * normal); // For uniform scaled objects
	v_normal_world_space = normalize(mat3(transpose(inverse(model_matrix))) * normal); 

	gl_Position = proj * view * vec4(position, 1.0f);
}