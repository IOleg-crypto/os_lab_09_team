add_library(window STATIC
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
)

# --- Qt Widgets ---
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- Qt automatic tools ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

target_include_directories(window PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(window PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    ipc_core
)
