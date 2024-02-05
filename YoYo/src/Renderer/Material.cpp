#include "Material.h"
#include "Core/Log.h"

#include "Resource/ResourceManager.h"

namespace yoyo
{
	template<>
	Ref<Material> ResourceManager::Load<Material>(const std::string& path)
	{
		ResourceId id = FileNameFromFullPath(path);

		auto material_it = m_material_cache.find(id);
		if(material_it != m_material_cache.end())
		{
			return material_it->second;
		}

		// TODO: Impl load from file
		// auto& material = Material::LoadFromAsset(path.c_str());
		// if(!material)
		// {
		// 	return nullptr;
		// }

		// m_material_cache[material->m_id] = material;
		// return material;

		return nullptr;
	}

	template<>
	void ResourceManager::Free<Material>(Ref<Material> resource)
	{
	}

    bool ResourceManager::OnMaterialCreated(Ref<Material> material)
    {
        // Check if shader already cached
        if (material->ID().empty())
        {
            // TODO: Generate string uuid
        }
        else
        {
            auto material_it = m_material_cache.find(material->ID());
            if (material_it != m_material_cache.end())
            {
                return true;
            }
        }

        // TODO: Check if there is space in cache
        m_material_cache[material->ID()] = material;
        return true;
    }

	Material::Material()
		:m_property_data(nullptr), m_property_size(0) {}

	Material::~Material() { free(m_property_data);}

	void Material::SetTexture(int index, Ref<Texture> texture)
	{
		if (textures.size() <= index)
		{
			textures.resize(index + 1);
		}

		textures[index] = texture;

		m_dirty |= MaterialDirtyFlags::TextureChange;
	}

	void Material::SetTexture(MaterialTextureType type, Ref<Texture> texture)
	{
		SetTexture((int)type, texture);
	}

	void Material::SetProperty(const std::string& name, void* data)
	{
		auto it = m_properties.find(name);

		if (it != m_properties.end())
		{
			MaterialProperty& prop = it->second;
			memcpy((char*)m_property_data + prop.offset, data, prop.size);
			m_dirty |= MaterialDirtyFlags::PropertyChange;
		}
		else
		{
			YERROR("Material has no such property: \"%s\"", name.c_str());
		}
	}

	void Material::AddProperty(const std::string& name, const MaterialProperty& property)
	{
		if (m_properties.find(name) != m_properties.end())
		{
			YERROR("Material already has property: \"%s\"", name.c_str());
			return;
		}
		m_properties[name] = property;

		// Check if property data has been allocated
		if (!m_property_data)
		{
			m_property_data = malloc(property.size);
			m_property_size = property.size;
			memset(m_property_data, 0, m_property_size);
			return;
		}

		// Realloc to accomodate new data
		m_property_data = realloc(m_property_data, m_property_size + property.size);
		m_property_size += property.size;
		memset((char*)m_property_data + property.offset, 0, property.size);
	}

	void Material::SetFloat(const std::string& name, float value)
	{
		SetProperty(name, (void*)&value);
	}

	void Material::SetVec4(const std::string& name, const Vec4& value)
	{
		SetProperty(name, (void*)&value);
	}
}