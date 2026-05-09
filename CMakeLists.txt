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

option(MDBOOK_BUILD_APP "Build GUI app target mdbook" ON)
option(MDBOOK_BUILD_TESTS "Build app tests" OFF)
option(MDBOOK_BUILD_PARSER_TOOL "Build utility parser" OFF)

set(MDBOOK_DEPS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/.deps-src" CACHE PATH
  "Shared directory for downloaded third-party sources")

set(MDBOOK_NEEDS_WX OFF)
set(MDBOOK_NEEDS_CMARK OFF)

if(MDBOOK_BUILD_APP OR MDBOOK_BUILD_TESTS)
    set(MDBOOK_NEEDS_WX ON)
    set(MDBOOK_NEEDS_CMARK ON)
endif()

if(MDBOOK_BUILD_PARSER_TOOL)
    set(MDBOOK_NEEDS_CMARK ON)
endif()

# Базовая GUI библиотека - wxWidgets
include(FetchContent)

if(MDBOOK_NEEDS_WX)
    set(MDBOOK_WX_SOURCE_DIR "${MDBOOK_DEPS_SOURCE_DIR}/wxwidgets.git")
    if(EXISTS "${MDBOOK_WX_SOURCE_DIR}/CMakeLists.txt")
        FetchContent_Declare(
          wxWidgets
          SOURCE_DIR "${MDBOOK_WX_SOURCE_DIR}"
          BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/wxwidgets-build"
          SUBBUILD_DIR "${CMAKE_BINARY_DIR}/_deps/wxwidgets-subbuild"
        )
    else()
        FetchContent_Declare(
          wxWidgets
          GIT_REPOSITORY "https://github.com/wxWidgets/wxWidgets.git"
          GIT_TAG "v3.3.2"
          SOURCE_DIR "${MDBOOK_WX_SOURCE_DIR}"
          BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/wxwidgets-build"
          SUBBUILD_DIR "${CMAKE_BINARY_DIR}/_deps/wxwidgets-subbuild"
        )
    endif()

    message(STATUS "wxWidgets source cache: ${MDBOOK_WX_SOURCE_DIR}")

    set(wxUSE_WEBVIEW "OFF")
    set(wxWidgets_USE_STATIC "OFF")
    set(wxBUILD_SHARED "ON")
    set(wxBUILD_MONOLITHIC "ON")

    FetchContent_GetProperties(wxWidgets)
    FetchContent_MakeAvailable(wxWidgets)
endif()

# Библиотека парсера Markdown - cmark
if(MDBOOK_NEEDS_CMARK)
    set(MDBOOK_CMARK_SOURCE_DIR "${MDBOOK_DEPS_SOURCE_DIR}/cmark.git")
    if(EXISTS "${MDBOOK_CMARK_SOURCE_DIR}/CMakeLists.txt")
        FetchContent_Declare(
          cmark
          SOURCE_DIR "${MDBOOK_CMARK_SOURCE_DIR}"
          BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/cmark-build"
          SUBBUILD_DIR "${CMAKE_BINARY_DIR}/_deps/cmark-subbuild"
        )
    else()
        FetchContent_Declare(
          cmark
          GIT_REPOSITORY "https://github.com/commonmark/cmark.git"
          GIT_TAG "0.31.2"
          SOURCE_DIR "${MDBOOK_CMARK_SOURCE_DIR}"
          BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/cmark-build"
          SUBBUILD_DIR "${CMAKE_BINARY_DIR}/_deps/cmark-subbuild"
        )
    endif()

    message(STATUS "cmark source cache: ${MDBOOK_CMARK_SOURCE_DIR}")

    FetchContent_GetProperties(cmark)
    FetchContent_MakeAvailable(cmark)
endif()

if(MDBOOK_BUILD_APP)
    # Ресурсный файл Windows (манифест, DPI и пр.)
    enable_language(RC)
    configure_file(
        ${CMAKE_SOURCE_DIR}/resources/app.rc.in
        ${CMAKE_BINARY_DIR}/app.rc
        COPYONLY
    )
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -I\"${wxwidgets_SOURCE_DIR}/include\"")

    # Сборка приложения
    AUX_SOURCE_DIRECTORY("src" SOURCE)
    add_executable(${MyAPP} ${SOURCE} ${CMAKE_BINARY_DIR}/app.rc)
    target_compile_definitions(${MyAPP} PRIVATE WXUSINGDLL __WXMSW__)
    # Скрыть консоль
    #target_link_options(${MyAPP} PRIVATE -mwindows -Wl,-subsystem,windows)
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
endif()

if(MDBOOK_BUILD_TESTS)
  include(${CMAKE_SOURCE_DIR}/tests/tests_targets.cmake)
endif()

if(MDBOOK_BUILD_PARSER_TOOL)
  include(${CMAKE_SOURCE_DIR}/parser/parser_tool.cmake)
endif()
