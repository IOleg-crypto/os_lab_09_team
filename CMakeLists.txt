cmake_minimum_required(VERSION 3.20)
project(os_9)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.9
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

add_library(imguilib STATIC
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/misc/cpp/imgui_stdlib.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
)
target_include_directories(imguilib PUBLIC include/imgui include/imgui/misc/cpp include/imgui/backends)
target_link_libraries(imguilib PUBLIC glfw)

find_package(OpenGL REQUIRED)

add_executable(os_9
        main/main.cpp
        core/Application.cpp
        core/Application.h
        managers/GuiRenderer.cpp
        managers/GuiRenderer.h
        managers/WindowManager.cpp
        managers/WindowManager.h
)

target_include_directories(os_9 PRIVATE
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/managers
)

target_link_libraries(os_9 PRIVATE imguilib glfw)

if (WIN32)
    target_link_libraries(os_9 PRIVATE opengl32)
    target_compile_definitions(os_9 PRIVATE _WIN32)
elseif(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(os_9 PRIVATE Threads::Threads ${OPENGL_LIBRARIES})
    target_compile_definitions(os_9 PRIVATE __linux__)
endif()
