#
# При вызове команды "cmake" (под win32) НЕОБХОДИМО указать тип сборки,
# например, для сборки в MinGW GCC указывается -
#
#    cmake -DCMAKE_BUILD_TYPE=Release ../ -G "MinGW Makefiles"
#
# потом:
#    cmake --build .
#
#
#
# https://cmake.org/Wiki/CMake_Useful_Variables
#
# wxWidgets - контроль (linux/msys2) параметров, необходимых для сборки:
#   wx-config --cxxflags --libs
#

cmake_minimum_required(VERSION 4.2.0)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
## Работать без установки программы
SET(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# Данные проекта
project(MyApp LANGUAGES CXX)
SET(MyAPP "markdown_dump")
set(PROJECT_AUTHOR "bigov")
set(PROJECT_AUTHOR_EMAIL "bigov@yandex.ru")
set(OperatingSystem "MS Windows")
set(MyApp_VERSION_MAJOR 0)
set(MyApp_VERSION_MINOR 1)

# Библиотека парсера Markdown
include(FetchContent)
#set(FETCHCONTENT_FULLY_DISCONNECTED OFF)
set(CMARK_LOCAL_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/cmark-src")
if(EXISTS "${CMARK_LOCAL_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Using local cmark source: ${CMARK_LOCAL_SOURCE_DIR}")
    FetchContent_Declare(
      cmark
      SOURCE_DIR "${CMARK_LOCAL_SOURCE_DIR}"
    )
else()
    FetchContent_Declare(
      cmark
      GIT_REPOSITORY "https://github.com/commonmark/cmark.git"
      GIT_TAG "0.31.2"
    )
endif()

FetchContent_GetProperties(cmark)
FetchContent_MakeAvailable(cmark)

set(SOURCE "src/main.cpp" )

# Сборка приложения
add_executable(${MyAPP} ${SOURCE})
target_link_libraries(${MyAPP} PRIVATE cmark)
