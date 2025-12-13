add_library(core STATIC
    Application.cpp
    Application.h
)

# --- Qt Widgets ---
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- Qt automatic tools ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/window
)

target_link_libraries(core PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    window
)
