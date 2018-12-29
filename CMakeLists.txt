# Project definition
cmake_minimum_required(VERSION 3.1)
project(Graphyte)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps")
# set(SOURCES ${SRC_DIR}/main.cpp)

set(SOURCES 
	"${SRC_DIR}/main.cpp"
	"${SRC_DIR}/Graphyte/Graphyte.cpp"
	"${SRC_DIR}/Input/Input.cpp"
	"${SRC_DIR}/IO/ModelLoader.cpp"
	"${SRC_DIR}/Logging/Console.cpp"

	"${SRC_DIR}/Objects/GameObject.cpp"
	"${SRC_DIR}/Objects/Transform.cpp"
	"${SRC_DIR}/Objects/Scene.cpp"

	"${SRC_DIR}/Rendering/Camera.cpp"
	"${SRC_DIR}/Rendering/MeshRenderer.cpp"
	"${SRC_DIR}/Rendering/Mesh.cpp"
	"${SRC_DIR}/Rendering/Screen.cpp"

	"${SRC_DIR}/Rendering/Material/Material.cpp"
	"${SRC_DIR}/Rendering/Material/Shader.cpp"
	"${SRC_DIR}/Rendering/Material/Texture.cpp"

	"${SRC_DIR}/Resources/ResourceManager.cpp"
)

set(HEADERS
	"${SRC_DIR}/Graphyte/Graphyte.h"
	"${SRC_DIR}/Audio/AudioEmitter.h"
	"${SRC_DIR}/Audio/AudioListener.h"
	"${SRC_DIR}/Cursor/Cursor.h"
	"${SRC_DIR}/Custom Behaviours/CameraOrbit.h"
	"${SRC_DIR}/Custom Behaviours/MoveAround.h"
	"${SRC_DIR}/Input/Input.h"
	"${SRC_DIR}/IO/ModelLoader.h"
	"${SRC_DIR}/Logging/Console.h"
	"${SRC_DIR}/Math/Math.h"

	"${SRC_DIR}/Objects/GameObject.h"
	"${SRC_DIR}/Objects/Transform.h"
	"${SRC_DIR}/Objects/Scene.h"

	"${SRC_DIR}/Rendering/Camera.h"
	"${SRC_DIR}/Rendering/MeshRenderer.h"
	"${SRC_DIR}/Rendering/Mesh.h"
	"${SRC_DIR}/Rendering/Screen.h"

	"${SRC_DIR}/Rendering/Material/Material.h"
	"${SRC_DIR}/Rendering/Material/Shader.h"
	"${SRC_DIR}/Rendering/Material/Texture.h"

	"${SRC_DIR}/Scripting/BehaviourScript.h"
)

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLM_ENABLE_EXPERIMENTAL")

# OpenGL
find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} "glfw" ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLAD
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# GLM
set(GLM_DIR "${LIB_DIR}/glm")
set(GLM_TEST_ENABLE OFF CACHE INTERNAL "Build unit tests")
add_subdirectory("${GLM_DIR}")
target_link_libraries(${PROJECT_NAME} "glm" "${GLM_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}/include")

# IMGUI
set(IMGUI_DIR "${LIB_DIR}/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}")

# SFML
set(SFML_DIR "${LIB_DIR}/SFML")
add_subdirectory("${SFML_DIR}")
target_link_libraries(${PROJECT_NAME} "sfml-audio" "${SFML_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${SFML_DIR}/include")
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_STD_LIBS ON CACHE INTERNAL "TRUE to statically link to the standard libraries, FALSE to use them as DLLs")

# set(SFML_STATIC_LIBRARIES TRUE)
# set(SFML_DIR "${LIB_DIR}/SFML")
# add_subdirectory("${SFML_DIR}")
# find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
# target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)

# ASSIMP
set(ASSIMP_DIR "${LIB_DIR}/assimp")
add_subdirectory("${ASSIMP_DIR}")
target_link_libraries(${PROJECT_NAME} "assimp" "${ASSIMP_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${ASSIMP_DIR}/include")

# STB
set(STB_DIR "${LIB_DIR}/stb")
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_DIR}")
