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
SET(MyAPP "mdbook")
set(PROJECT_AUTHOR "bigov")
set(PROJECT_AUTHOR_EMAIL "bigov@yandex.ru")
set(OperatingSystem "MS Windows")
set(MyApp_VERSION_MAJOR 0)
set(MyApp_VERSION_MINOR 1)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWXUSINGDLL -D__WXMSW__ -mwindows -Wl,-subsystem,windows")

# Базовая библиотека
include(FetchContent)
set(WXWIDGETS_LOCAL_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/wxwidgets-src")
if(EXISTS "${WXWIDGETS_LOCAL_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Using local wxWidgets source: ${WXWIDGETS_LOCAL_SOURCE_DIR}")
    # Avoid network/git for local rebuilds when source is already populated.
    set(FETCHCONTENT_FULLY_DISCONNECTED ON)
    FetchContent_Declare(
      wxWidgets
      SOURCE_DIR "${WXWIDGETS_LOCAL_SOURCE_DIR}"
    )
else()
    FetchContent_Declare(
      wxWidgets
      GIT_REPOSITORY "https://github.com/wxWidgets/wxWidgets.git"
      GIT_TAG "v3.3.2"
    )
endif()
set(wxUSE_WEBVIEW "OFF")
#set(wxWidgets_USE_STATIC 1)
set(wxBUILD_SHARED "ON")
set(wxBUILD_MONOLITHIC "ON")

FetchContent_GetProperties(wxWidgets)
FetchContent_MakeAvailable(wxWidgets)

# Allow FetchContent to download cmark on first configure.
set(FETCHCONTENT_FULLY_DISCONNECTED OFF)

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

configure_file(resources/app.rc.in ${CMAKE_BINARY_DIR}/app.rc COPYONLY)
set_source_files_properties("${CMAKE_BINARY_DIR}/app.rc" PROPERTIES
    COMPILE_FLAGS "-I${wxwidgets_SOURCE_DIR}/include"
)

set(SOURCE "${CMAKE_BINARY_DIR}/app.rc"
   "src/main.cpp"
   "src/app_frame.h"
   "src/app_frame.cpp"
   "src/txt_ctl.h"
   "src/txt_ctl.cpp"
   "src/md_parser.h"
    "src/md_parser.cpp"
)

# Сборка приложения
add_executable(${MyAPP} ${SOURCE})
target_link_libraries(${MyAPP} PRIVATE wxWidgets::wxWidgets cmark)

# Добавление .dll библиотеки в каталог запуска
SET(BIN_LIB "${CMAKE_BINARY_DIR}/_deps/wxwidgets-build/lib/gcc_x64_dll")
add_custom_command( TARGET "${MyAPP}" POST_BUILD
  COMMAND ${CMAKE_COMMAND}
    -E copy "${BIN_LIB}/wxmsw332ud_gcc_x64_custom.dll" "${CMAKE_BINARY_DIR}/"
  COMMAND ${CMAKE_COMMAND}
    -E make_directory "${CMAKE_BINARY_DIR}/assets"
  COMMAND ${CMAKE_COMMAND}
    -E copy "${CMAKE_SOURCE_DIR}/resources/icon.png" "${CMAKE_BINARY_DIR}/assets/"
)
