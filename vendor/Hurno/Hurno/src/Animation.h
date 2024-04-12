#pragma once

#include <string>
#include <vector>

#include "Asset.h"

namespace hro
{
    template<typename T>
    struct HAPI Channel
    {
        uint32_t bone_index = 0;
        float time = 0;
        T data = {};

        const uint64_t SizeOfData() const {return sizeof(T);}
    };

    class HAPI Animation : public Asset
    {
    public:
        std::string name;
        float ticks;
        float ticks_per_second;

        std::vector<Channel<float[3]>> position_channels = {};
        std::vector<Channel<float[4]>> rotation_channels = {};
        std::vector<Channel<float[3]>> scale_channels = {};

        virtual void ParseInfo(AssetInfo* out) override;
    protected:
        virtual void PackImpl(const AssetInfo* info) override;
        virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) override;
    };
}
