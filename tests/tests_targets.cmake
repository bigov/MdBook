# Utility targets from tests module.
get_filename_component(MDBOOK_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

add_executable(test_parser
    ${CMAKE_CURRENT_LIST_DIR}/test_parser.cpp
    ${MDBOOK_ROOT_DIR}/src/md_parser.h
    ${MDBOOK_ROOT_DIR}/src/md_parser.cpp
)
target_include_directories(test_parser PRIVATE ${MDBOOK_ROOT_DIR}/src)
target_compile_definitions(test_parser PRIVATE
    TEST_MD_PATH="${MDBOOK_ROOT_DIR}/tests/docs-tests/test.md"
)
target_link_libraries(test_parser PRIVATE wxWidgets::wxWidgets cmark)
target_link_options(test_parser PRIVATE -mconsole -Wl,-subsystem,console)

add_executable(test_parser_debug
    ${CMAKE_CURRENT_LIST_DIR}/test_parser_debug.cpp
    ${MDBOOK_ROOT_DIR}/src/md_parser.h
    ${MDBOOK_ROOT_DIR}/src/md_parser.cpp
    ${MDBOOK_ROOT_DIR}/src/tpls.h
)
target_include_directories(test_parser_debug PRIVATE ${MDBOOK_ROOT_DIR}/src)
target_link_libraries(test_parser_debug PRIVATE wxWidgets::wxWidgets cmark)
target_link_options(test_parser_debug PRIVATE -mconsole -Wl,-subsystem,console)

add_executable(test_inline_styles
    ${CMAKE_CURRENT_LIST_DIR}/test_inline_styles.cpp
    ${MDBOOK_ROOT_DIR}/src/md_parser.h
    ${MDBOOK_ROOT_DIR}/src/md_parser.cpp
)
target_include_directories(test_inline_styles PRIVATE ${MDBOOK_ROOT_DIR}/src)
target_link_libraries(test_inline_styles PRIVATE wxWidgets::wxWidgets cmark)
target_link_options(test_inline_styles PRIVATE -mconsole -Wl,-subsystem,console)

enable_testing()
add_test(NAME parser_heading_test COMMAND test_parser)
add_test(NAME parser_inline_styles_test COMMAND test_inline_styles)
