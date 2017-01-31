cmake_minimum_required(VERSION 2.8.11)

project(bebuch_linescan)


include(CTest)

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(
	PROJ            bebuch_mitrax
	GIT_REPOSITORY  "https://github.com/bebuch/mitrax.git"
	GIT_TAG         "bachelorarbeit"
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

add_subdirectory(${bebuch_mitrax_SOURCE_DIR})
include_directories("${bebuch_mitrax_SOURCE_DIR}/include")


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Core Gui Test Widgets)
set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_FEATURES "")
set_property(TARGET Qt5::Gui PROPERTY INTERFACE_COMPILE_FEATURES "")
set_property(TARGET Qt5::Test PROPERTY INTERFACE_COMPILE_FEATURES "")
set_property(TARGET Qt5::Widgets PROPERTY INTERFACE_COMPILE_FEATURES "")


include_directories("../include")
file(GLOB sources "../src/*.cpp")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(Boost_LIBRARY_DIR ${Boost_INCLUDE_DIRS}/stage/lib)


find_package(Threads)


find_package(PNG)
include_directories(${PNG_INCLUDE_DIR})


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")


add_executable(linescan ${sources})
qt5_use_modules(linescan Core Gui Test Widgets)
target_link_libraries(linescan ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(linescan ueye_api)
target_link_libraries(linescan ${Boost_LIBRARY_DIR}/libboost_system.so)
target_link_libraries(linescan ${PNG_LIBRARY})
