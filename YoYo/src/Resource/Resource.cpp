#include "Resource.h"

const std::string yoyo::FileNameFromFullPath(const std::string& full_path)
{
    auto first = full_path.find_last_of("/\\") + 1;
    auto last = full_path.find_last_of(".");
    
    auto it = last - first;
    return full_path.substr(first, last - first);
}

