cmake_minimum_required(VERSION 3.16)
project(HyprDisplays VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Set up Qt6
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/displaymanager.cpp
    src/displaywidget.cpp
    src/hyprlandinterface.cpp
    src/configmanager.cpp
    src/visualmonitorwidget.cpp
    src/monitorgraphicsview.cpp
)

set(HEADERS
    src/mainwindow.h
    src/displaymanager.h
    src/displaywidget.h
    src/hyprlandinterface.h
    src/configmanager.h
    src/visualmonitorwidget.h
    src/monitorgraphicsview.h
)

set(UI_FILES
    src/mainwindow.ui
)

# Add Qt resource file
qt_add_resources(hyprdisplays_resources resources.qrc)

# Create executable
add_executable(hyprdisplays ${SOURCES} ${HEADERS} ${UI_FILES} ${hyprdisplays_resources})

# Link Qt6 libraries
target_link_libraries(hyprdisplays
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# Install target
install(TARGETS hyprdisplays DESTINATION bin)

# Install desktop file
install(FILES hyprdisplays.desktop DESTINATION share/applications)

# Install icon
install(FILES hyprdisplays.svg DESTINATION share/icons/hicolor/scalable/apps)

# Install headers
install(FILES
    src/mainwindow.h
    src/displaymanager.h
    src/displaywidget.h
    src/hyprlandinterface.h
    src/visualmonitorwidget.h
    src/configmanager.h
    src/monitorgraphicsview.h
    DESTINATION include
) 