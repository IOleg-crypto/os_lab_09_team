cmake_minimum_required(VERSION 3.29)

project(os_9 LANGUAGES CXX)

# --- Qt automatic tools ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Qt Widgets ---
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_subdirectory(core)
add_subdirectory(window)
add_subdirectory(ipc)

# --- Sources ---
set(APP_MAIN
    main/main.cpp
)

# --- Executable ---
qt_add_executable(os_9
    MANUAL_FINALIZATION
    ${APP_MAIN}
)

target_link_libraries(os_9 PRIVATE
    core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# --- Windows Libraries ---
# ВАЖЛИВО: Підключаємо бібліотеку для UUID (Rpcrt4), тільки якщо це Windows
if(WIN32)
    target_link_libraries(os_9 PRIVATE Rpcrt4)
endif()

# --- Finalize for Qt 6 ---
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(os_9)
endif()
