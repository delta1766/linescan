cmake_minimum_required(VERSION 2.8.11)

project(bebuch_linescan)


include(CTest)

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                bebuch_mitrax
	GIT_REPOSITORY      "https://github.com/bebuch/mitrax.git"
	GIT_TAG             "master"
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Test Widgets)



include_directories("../include")
file(GLOB sources "../src/*.cpp")

add_executable(linescan ${sources})
qt5_use_modules(linescan Core Gui Test Widgets)
