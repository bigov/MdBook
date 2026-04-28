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
FetchContent_Declare(
        wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.3.2
)
set(wxUSE_WEBVIEW "OFF")
#set(wxWidgets_USE_STATIC 1)
set(wxBUILD_SHARED "ON")
set(wxBUILD_MONOLITHIC "ON")

FetchContent_GetProperties(wxWidgets)
FetchContent_MakeAvailable(wxWidgets)

# Сборка приложения
configure_file(resources/app.rc.in ${CMAKE_BINARY_DIR}/app.rc COPYONLY)
configure_file(resources/icon.png ${CMAKE_BINARY_DIR}/icon.png COPYONLY)

set(SOURCE "src/main.cpp" "${CMAKE_BINARY_DIR}/app.rc")
add_executable(${MyAPP} ${SOURCE})

# Путь к заголовкам wxWidgets нужен windres при компиляции app.rc (для wx/msw/wx.rc)
set_source_files_properties("${CMAKE_BINARY_DIR}/app.rc" PROPERTIES
    COMPILE_FLAGS "-I${wxwidgets_SOURCE_DIR}/include"
)
 
#target_link_libraries(${MyAPP} PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(${MyAPP} PRIVATE wxWidgets::wxWidgets)

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
