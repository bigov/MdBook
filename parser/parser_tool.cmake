# Utility target from parser module.
add_executable(parser
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)
target_link_libraries(parser PRIVATE cmark)
target_link_options(parser PRIVATE -mconsole -Wl,-subsystem,console)
