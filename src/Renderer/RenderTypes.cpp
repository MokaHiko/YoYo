#include "RenderTypes.h"
#include "Core/Assert.h"

namespace yoyo
{
    uint64_t FormatToSize(Format format)
    {
        static std::array<uint64_t, (int)Format::Maximum> FormatSizeMap = {0};
        FormatSizeMap[(int)Format::R32G32_SFLOAT] = 8;

        FormatSizeMap[(int)Format::R32G32B32_SFLOAT] = 12;

        FormatSizeMap[(int)Format::R32G32B32A32_SFLOAT] = 16;
        FormatSizeMap[(int)Format::R32G32B32A32_SINT] = 16;

		int index = (int)format;
		YASSERT(index < FormatSizeMap.size(), "Invlaid Format!");

		return FormatSizeMap[index];
    }
}