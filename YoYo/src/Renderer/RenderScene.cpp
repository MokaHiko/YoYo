#include "RenderScene.h"

yoyo::RenderScene::RenderScene() 
{
	forward_pass = CreateRef<MeshPass>();
	transparent_forward_pass = CreateRef<MeshPass>();
	shadow_pass = CreateRef<MeshPass>();
}

yoyo::RenderScene::~RenderScene() {}

