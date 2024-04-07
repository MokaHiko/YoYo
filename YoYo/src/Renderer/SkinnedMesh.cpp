#include "SkinnedMesh.h"

#include <Hurno.h>

#include "Core/Log.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceEvent.h"

namespace yoyo
{
	template<>
	YAPI Ref<SkeletalHierarchy> ResourceManager::Load<SkeletalHierarchy>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& sk_cache = Cache<SkeletalHierarchy>();
		auto sk_it = std::find_if(sk_cache.begin(), sk_cache.end(), [&](const auto& it) {
			return it.second->name == name;
			});

		if (sk_it != sk_cache.end())
		{
			return sk_it->second;
		}

		YWARN("[Cache Miss][SkeletalHierarchy]: %s", name.c_str());

		Ref<SkeletalHierarchy> sk_hierarchy = SkeletalHierarchy::LoadFromAsset(path.c_str(), name);
		sk_cache[sk_hierarchy->Id()] = sk_hierarchy;
		return sk_hierarchy;
	}

	template<>
	YAPI void ResourceManager::Free<SkeletalHierarchy>(Ref<SkeletalHierarchy> resource)
	{
		// TODO: Free resource
	}

	template<>
	YAPI Ref<SkinnedMesh> ResourceManager::Load<SkinnedMesh>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& mesh_cache = Cache<SkinnedMesh>();
		auto mesh_it = std::find_if(mesh_cache.begin(), mesh_cache.end(), [&](const auto& it) {
			return it.second->name == name;
			});

		if (mesh_it != mesh_cache.end())
		{
			return mesh_it->second;
		}

		// TODO: Load from asset file
		YWARN("[Cache Miss][SkinnedMesh]: %s", name.c_str());

		return nullptr;
	}

	template<>
	YAPI void ResourceManager::Free<SkinnedMesh>(Ref<SkinnedMesh> resource)
	{
		// TODO: Free resource
	}

	Ref<SkeletalHierarchy> SkeletalHierarchy::Create(const std::string& name)
	{
		Ref<SkeletalHierarchy> skeletal_hierarchy = CreateRef<SkeletalHierarchy>();
		skeletal_hierarchy->name = name;

		EventManager::Instance().Dispatch(CreateRef<SkeletalHierarchyCreatedEvent>(skeletal_hierarchy));
		return skeletal_hierarchy;
	}

	void CopyHroHierarchy(hro::Node* hro_node, SkeletalNode* node, hro::SkeletalMesh* sk_mesh)
	{
		if(hro_node->joint_index == hro::INVALID_JOINT_INDEX)
		{
			// Invalid node
		}

		// Copy data
		node->name = hro_node->name_buffer;
		node->joint_index = hro_node->joint_index;
		memcpy(&node->transform, hro_node->transformation, sizeof(float) * 16);

		// Create children and recursive call 
		node->children.resize(hro_node->ChildrenCount());
		for (int i = 0; i < hro_node->ChildrenCount(); i++)
		{
			SkeletalNode* child_node = node->children[i] = YNEW SkeletalNode();
			node->children[i] = child_node;

			hro::Node* hro_child_node = hro_node->GetChild(i, sk_mesh->Root());
			CopyHroHierarchy(hro_child_node, child_node, sk_mesh);
		}
	}

	Ref<SkeletalHierarchy> SkeletalHierarchy::LoadFromAsset(const char* asset_path, const std::string& name)
	{
		hro::SkeletalMesh sk_mesh = {};
		hro::AssetInfo sk_info = {};
		if(sk_mesh.Load(asset_path))
		{
			sk_mesh.ParseInfo(&sk_info);
			sk_mesh.Unpack(&sk_info, nullptr);

			hro::Node* hro_node = sk_mesh.Root();

			// Create and copy skeletal hierarchy
            const std::string sk_mesh_name = name.empty() ? FileNameFromFullPath(asset_path) : name;
			Ref<SkeletalHierarchy> sk_hierarchy = SkeletalHierarchy::Create(sk_mesh_name);
			SkeletalNode* node = sk_hierarchy->root = YNEW SkeletalNode;
			CopyHroHierarchy(hro_node, node, &sk_mesh);

			return sk_hierarchy;
		}

		return nullptr;
	}

    void SkeletalHierarchy::Traverse(std::function<void(const SkeletalNode*)> fn) const
    {
		if(!root)
		{
			return;
		}

        if(!fn)
        {
            return;
        }

		TraverseRecursive(root, fn);
    }

	const void SkeletalHierarchy::TraverseRecursive(const SkeletalNode* node, std::function<void(const SkeletalNode*)> fn) const
	{
		if(!node)
		{
			return;
		}

		fn(node);

		for(SkeletalNode* child : node->children)
		{
			TraverseRecursive(child, fn);
		}
	}
}