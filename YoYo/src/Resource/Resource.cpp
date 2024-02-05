#include "Resource.h"

const std::string yoyo::FileNameFromFullPath(const std::string& full_path)
{
    return full_path.substr(full_path.find_last_of("/\\") + 1);
}

