#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifndef TEST_OUT_PATH
#define TEST_OUT_PATH "test_parser_out.txt"
#endif

#include <wx/init.h>

#include "cmark.h"
#include "md_parser.h"

#ifndef TEST_MD_PATH
#define TEST_MD_PATH "../../resources/docs-tests/test.md"
#endif

int main()
{
    wxInitializer wx;

    std::ifstream f(TEST_MD_PATH);
    if (!f) {
        std::cerr << "Cannot open: " TEST_MD_PATH "\n";
        return 1;
    }
    const std::string md(std::istreambuf_iterator<char>(f),
                         std::istreambuf_iterator<char>{});

    cmark_node *doc = cmark_parse_document(md.c_str(), md.size(), CMARK_OPT_SOURCEPOS);
    if (!doc) {
        std::cerr << "cmark_parse_document returned null\n";
        return 1;
    }

    std::ostringstream out;
    process_node(doc, out);
    cmark_node_free(doc);

    const std::string result = out.str();
    std::cout << result;
    std::ofstream fout(TEST_OUT_PATH);
    fout << result;
    return 0;
}
