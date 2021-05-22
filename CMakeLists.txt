# Original code from Skyedeving at
# https://github.com/AttorneyOnline/AO2-Client/blob/master/CMakeLists.txt

# Configure cmake
cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0076 NEW) # silence warning

project(DanganronpaOnline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# DRO
add_executable(Danganronpa_Online resources.qrc)

# WIN32
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_property(TARGET Danganronpa_Online PROPERTY WIN32_EXECUTABLE true)
        set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/res/logo_dro.rc")
        target_sources(Danganronpa_Online PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
    endif()
endif()

# Target Include
target_include_directories(Danganronpa_Online PRIVATE include)

# Target Lib
find_package(Qt5 COMPONENTS Core Gui Network Widgets REQUIRED)
target_link_directories(Danganronpa_Online PRIVATE lib)
target_link_libraries(Danganronpa_Online PRIVATE Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
    bass bassopus discord-rpc)
target_compile_definitions(Danganronpa_Online PRIVATE DISCORD)

# Subdirectories
# add_subdirectory(test) # We currently do not support tests
add_subdirectory(src)
add_subdirectory(include)
