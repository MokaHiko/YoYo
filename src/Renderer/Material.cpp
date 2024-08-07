#include "Material.h"
#include "Core/Log.h"

#include "Texture.h"
#include "Shader.h"
#include "Resource/ResourceManager.h"

namespace yoyo
{
	template<>
	YAPI Ref<Material> ResourceManager::Load<Material>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& material_cache = Cache<Material>();
		auto material_it = std::find_if(material_cache.begin(), material_cache.end(), [&](const auto& it){
			return it.second->name == name;
		});
		
		if(material_it != material_cache.end())
		{
			return material_it->second;
		}

        // TODO: Load from asset file
        YWARN("[Cache Miss][Material]: %s", name.c_str());

		return nullptr;
	}

	template<>
	YAPI void ResourceManager::Free<Material>(Ref<Material> resource)
	{
	}

	Material::Material()
		:m_property_data(nullptr), m_property_size(0) {}

	Material::~Material() 
	{ 
		if(m_property_data != nullptr)
		{
			YFree(m_property_data);
		}
	}

	const Ref<Texture> Material::GetTexture(int index) const
	{
		if(textures.size() <= index)
		{
			return nullptr;
		}

		return textures[index];
	}

void Material::SetTexture(int index, Ref<Texture> texture)
	{
		if (textures.size() <= index)
		{
			textures.resize(index + 1);
		}

		textures[index] = texture;

		m_dirty |= MaterialDirtyFlags::Texture;
	}

	void Material::SetTexture(MaterialTextureType type, Ref<Texture> texture)
	{
		SetTexture((int)type, texture);
	}

	void Material::SetProperty(const std::string& name, const void* data)
	{
		auto it = m_properties.find(name);

		if (it != m_properties.end())
		{
			MaterialProperty& prop = it->second;
			memcpy((char*)m_property_data + prop.offset, data, prop.size);
			m_dirty |= MaterialDirtyFlags::Property;
		}
		else
		{
			YERROR("Material has no such property: \"%s\"", name.c_str());
		}
	}

    void Material::GetProperty(const std::string &name, void *data) const
    {
		auto it = m_properties.find(name);

		if (it != m_properties.end())
		{
			const MaterialProperty& prop = it->second;
			memcpy(data, (char*)m_property_data + prop.offset, prop.size);
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
			m_property_data = YAllocate(property.size, yoyo::MemoryTag::Resource);
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