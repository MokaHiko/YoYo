#pragma once

#include "../src/Model.h"
#include "../src/Mesh.h"
#include "../src/Texture.h"
#include "../src/SkeletalMesh.h"

namespace hro
{
	HAPI bool ConvertTexture(const char* path, const char* out_path = "");
	HAPI bool ConvertTexture(void* buffer, uint32_t buffer_size, const char* path, const char* out_path = "");
	HAPI bool ConvertModel(const char* path, const char* out_path = "", bool flip_uvs = false);
}