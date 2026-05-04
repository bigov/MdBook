#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <wx/init.h>
#include "cmark.h"
#include "md_parser.h"
#include "wx/string.h"

#ifndef TEST_MD_PATH
#define TEST_MD_PATH "../../resources/docs-tests/test.md"
#endif

int main()
{
    wxInitializer wx;

    // Простой тест: "During spring training in **2016**, он..."
    const char *simple_md = "During spring training in **2016**, he was diagnosed.";
    
    std::cout << "=== DEBUG TEST ===" << std::endl;
    std::cout << "Input MD: " << simple_md << std::endl;
    std::cout << std::endl;

    cmark_node *doc = cmark_parse_document(simple_md, strlen(simple_md), CMARK_OPT_SOURCEPOS);
    if (!doc) {
        std::cerr << "cmark_parse_document returned null\n";
        return 1;
    }

    // Печатаем структуру дерева
    std::cout << "=== CMARK TREE STRUCTURE ===" << std::endl;
    cmark_node *cur = doc;
    int depth = 0;
    
    std::function<void(cmark_node*, int)> print_tree = [&](cmark_node *node, int d) {
        if (!node) return;
        for (int i = 0; i < d; i++) std::cout << "  ";
        
        const char *type_name = "";
        switch (cmark_node_get_type(node)) {
            case CMARK_NODE_DOCUMENT: type_name = "DOCUMENT"; break;
            case CMARK_NODE_PARAGRAPH: type_name = "PARAGRAPH"; break;
            case CMARK_NODE_STRONG: type_name = "STRONG"; break;
            case CMARK_NODE_TEXT: type_name = "TEXT"; break;
            default: type_name = "OTHER"; break;
        }
        std::cout << type_name;
        
        const char *lit = cmark_node_get_literal(node);
        if (lit) {
            std::cout << " [" << lit << "]";
        }
        std::cout << std::endl;
        
        if (cmark_node_first_child(node)) {
            print_tree(cmark_node_first_child(node), d + 1);
        }
        if (cmark_node_next(node)) {
            print_tree(cmark_node_next(node), d);
        }
    };
    
    print_tree(doc, 0);

    std::cout << "\n=== PROCESS_NODE OUTPUT ===" << std::endl;
    std::ostringstream out;
    process_node(doc, out);
    cmark_node_free(doc);

    const std::string result = out.str();
    std::cout << result << std::endl;
    
    // Проверка наличия тега <text fontweight="700">2016</text>
    std::cout << "\n=== VALIDATION ===" << std::endl;
    if (result.find("2016") != std::string::npos) {
        std::cout << "✓ Found '2016' in output" << std::endl;
    } else {
        std::cout << "✗ '2016' NOT found in output" << std::endl;
    }
    
    if (result.find("fontweight=\"700\"") != std::string::npos) {
        std::cout << "✓ Found strong formatting (fontweight=\"700\")" << std::endl;
    } else {
        std::cout << "✗ Strong formatting NOT found" << std::endl;
    }
    
    if (result.find("<text>") != std::string::npos) {
        std::cout << "✓ Found XML tags" << std::endl;
    } else {
        std::cout << "✗ NO XML tags found (only raw text?)" << std::endl;
    }

    return 0;
}
