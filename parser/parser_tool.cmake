# Utility target from parser module.
add_executable(parser
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)
target_link_options(parser PRIVATE -mconsole -Wl,-subsystem,console)
target_link_libraries(parser PRIVATE cmark)

add_custom_command( TARGET parser POST_BUILD
      COMMAND ${CMAKE_COMMAND}
        -E copy "${CMAKE_CURRENT_LIST_DIR}/tst-docs/2.txt" "${CMAKE_BINARY_DIR}/"
)
