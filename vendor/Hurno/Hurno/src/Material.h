#pragma once

#include "Defines.h"
#include "Asset.h"

#include <string>

namespace hro
{
  struct MaterialInfo : public AssetInfo
  {
      // Returns the size of raw data when unpacked
      virtual const uint64_t UnpackedSize() const { return 0; }
  };

  class HAPI Material : public Asset
  {
  public:
    Material();
    ~Material();

    // Parses the model meta data into out 
    virtual void ParseInfo(AssetInfo* out) override;
  public:
    std::string name;

    float ambient_color[3];
    float diffuse_color[3];
    float specular_color[3];

    std::string diffuse_texture_path;
    std::string specular_texture_path;
    std::string normal_texture_path;
  protected:
    virtual void PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size) override;
    virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) override;
  };
}
