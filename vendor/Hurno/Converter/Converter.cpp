#include <Hurno.h>
#include <SkeletalMesh.h>

void PrintFullPath(char* partialPath)
{
	char full[_MAX_PATH];
	if (_fullpath(full, partialPath, _MAX_PATH) != NULL)
		printf("Full path is: %s\n", full);
	else
		printf("Invalid path\n");
}

int main()
{
	PrintFullPath(".\\");

	//printf("\n\n");
	//hro::ConvertModel("assets/models/plane.fbx", "assets/models/plane.yo", true);
	//printf("\n\n");
	//hro::ConvertModel("assets/models/pine.obj", "assets/models/pine.yo", true);
	//printf("\n\n");
	//hro::ConvertModel("assets/models/cube.obj", "assets/models/cube.yo");
	//
	//printf("\n\n");
	//hro::ConvertModel("assets/models/character-digger.fbx", "assets/models/character-digger.yo", true);

	//printf("\n\n");
	//hro::ConvertModel("assets/models/Peasant.fbx", "assets/models/Peasant.yo", false);

	//printf("\n\n");
	//hro::ConvertModel("assets/models/Humanoid.dae", "assets/models/Humanoid.yo", false);
	//hro::ConvertModel("assets/models/HipHopDancing.dae", "assets/models/Humanoid.yo", false);
	hro::ConvertModel("assets/models/Running.dae", "assets/models/Running.yo", false);

	//printf("\n\n");
	//hro::ConvertModel("assets/models/mutant.fbx", "assets/models/mutant.yo", true);

	//printf("\n\n");
	// hro::ConvertTexture("assets/textures/people_texture_map.png", "assets/textures/people_texture_map.yo");
	//hro::ConvertTexture("assets/textures/colormap.png", "assets/textures/colormap.yo");
	//hro::ConvertTexture("assets/textures/prototype_512x512_white.png", "assets/textures/prototype_512x512_white.yo");

	//hro::SkeletalMesh sk_mesh = {};
	//hro::AssetInfo sk_info = {};
	//sk_mesh.Load("assets/skeletal_meshes/test.yskmesh");

	//sk_mesh.ParseInfo(&sk_info);
	//sk_mesh.Unpack(&sk_info, nullptr);


	return 0;
}