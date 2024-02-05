#pragma once

namespace yoyo
{
    // The unique identifier of a resource
    using ResourceId = std::string;
    
    // ex. some_folder/my_file.extension => my_file.extension
    const std::string FileNameFromFullPath(const std::string& full_path);
}