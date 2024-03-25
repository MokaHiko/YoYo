#include "ResourceEvent.h"

namespace yoyo
{
	TextureCreatedEvent::TextureCreatedEvent(Ref<Texture> new_texture)
		:texture(new_texture){}

	ShaderCreatedEvent::ShaderCreatedEvent(Ref<Shader> new_shader)
		:shader(new_shader){}

	MaterialCreatedEvent::MaterialCreatedEvent(Ref<Material> new_material)
		:material(new_material){}
}