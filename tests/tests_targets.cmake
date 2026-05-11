# Utility targets from tests module.
get_filename_component(MDBOOK_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

add_executable(test_txt_ctl_reload_styles
    ${CMAKE_CURRENT_LIST_DIR}/test_txt_ctl_reload_styles.cpp
    ${MDBOOK_ROOT_DIR}/src/txt_ctl.h
    ${MDBOOK_ROOT_DIR}/src/txt_ctl.cpp
)
target_include_directories(test_txt_ctl_reload_styles PRIVATE ${MDBOOK_ROOT_DIR}/src)
target_compile_definitions(test_txt_ctl_reload_styles PRIVATE
    TEST_MD_RELOAD_PATH="${MDBOOK_ROOT_DIR}/tests/docs-tests/test.md"
)
target_link_libraries(test_txt_ctl_reload_styles PRIVATE wxWidgets::wxWidgets cmark)
target_link_options(test_txt_ctl_reload_styles PRIVATE -mconsole -Wl,-subsystem,console)

enable_testing()
add_test(NAME txt_ctl_reload_styles_test COMMAND test_txt_ctl_reload_styles)
