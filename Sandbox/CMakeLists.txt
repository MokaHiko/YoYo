cmake_minimum_required(VERSION 3.0.0)
project(Sandbox VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
	src/ECS/Entity.h
	src/ECS/Entity.cpp
	src/ECS/EntityImpl.h
	src/ECS/Scene.h
	src/ECS/Scene.cpp

	src/ECS/Components/Components.h
	src/ECS/Components/Components.cpp

	src/ECS/Components/RenderableComponents.h
	src/ECS/Components/RenderableComponents.cpp

	src/Scripts/NativeScript.h
	src/Scripts/NativeScript.cpp

	src/SceneGraph/SceneGraph.h
	src/SceneGraph/SceneGraph.cpp

	src/Sandbox.cpp

	src/Scripts/CameraController.h
	src/Scripts/CameraController.cpp

	src/Scripts/Destructable.h
	src/Scripts/Destructable.cpp

	src/Scripts/Sun.h
	src/Scripts/Sun.cpp

	src/Scripts/Villager.h
	src/Scripts/Villager.cpp
)
add_subdirectory(vendor/entt)
target_link_libraries(${PROJECT_NAME} YoYo EnTT)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

add_custom_target(copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/assets
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
	COMMENT "Copying resources into bin and binary folder."
	COMMAND cmd /c ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders/compile_shaders.bat
	COMMENT "Compiling shaders.")

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_dependencies(${PROJECT_NAME} copy_assets)