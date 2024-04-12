cmake_minimum_required(VERSION 3.2.0)
project(Hurno VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/json)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/lz4/build/cmake)

set(CMAKE_CXX_STANDARD 17)
add_library(${PROJECT_NAME} 
						src/Defines.h

						src/Asset.h 
						src/Asset.cpp

						src/Texture.h 
						src/Texture.cpp

						src/Mesh.h
						src/Mesh.cpp 

						src/Model.h
						src/Model.cpp 

						src/Material.h
						src/Material.cpp 

						src/SkeletalMesh.h
						src/SkeletalMesh.cpp
						
						src/Animation.h
						src/Animation.cpp

						src/Impl/AssimpImpl.h
						src/Impl/AssimpImpl.cpp

						include/Hurno.h 
						include/Hurno.cpp
						)
target_include_directories(${PROJECT_NAME} PUBLIC include src)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json stb_image lz4_static assimp)

add_compile_definitions(H_EXPORT)

if (WIN32)
    add_compile_definitions(H_PLATFORM_WIN32)
endif()