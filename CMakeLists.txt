# Project definition
cmake_minimum_required(VERSION 3.1)
project(Graphyte)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps")

file(GLOB_RECURSE SOURCES
	"${SRC_DIR}/*.c*"
)

file(GLOB_RECURSE HEADERS
	"${SRC_DIR}/*.h"
)

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${SRC_DIR}/*.c*"
    "${SRC_DIR}/*.h*"
)

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLM_ENABLE_EXPERIMENTAL")

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${SRC_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

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
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}/glm")

# IMGUI
set(IMGUI_DIR "${LIB_DIR}/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}")

# You can probably include files easier some other way that i dont know of
file(GLOB IMGUI_SOURCES
    "${IMGUI_DIR}/*.h"
    "${IMGUI_DIR}/*.cpp"
)
	
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE "IMGUI_IMPL_OPENGL_LOADER_GLAD")

# SFML
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "${LIB_DIR}/SFML")
add_subdirectory("${SFML_DIR}")
target_link_libraries(${PROJECT_NAME} "sfml-audio" "${SFML_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${SFML_DIR}/include")
set(SFML_USE_STATIC_STD_LIBS FALSE CACHE INTERNAL "TRUE to statically link to the standard libraries, FALSE to use them as DLLs")

# ASSIMP
set(ASSIMP_DIR "${LIB_DIR}/assimp")
add_subdirectory("${ASSIMP_DIR}")
target_link_libraries(${PROJECT_NAME} "assimp" "${ASSIMP_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${ASSIMP_DIR}/include")

# STB
set(STB_DIR "${LIB_DIR}/stb")
target_include_directories(${PROJECT_NAME} PUBLIC "${STB_DIR}")
