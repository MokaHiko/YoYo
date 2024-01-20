#version 460

layout(set = 0, binding = 0) uniform sampler2D input_texture;

layout(location = 3) in vec2 v_uv;

layout(location = 0) out vec4 frag_color;

void main()
{
	frag_color = texture(input_texture, v_uv);
}