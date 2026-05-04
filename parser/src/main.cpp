#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
extern "C" {
#include "cmark.h"
}

class NParser {
private:
    cmark_node* node;

public:
    NParser(const std::string& input)
    {
        node = cmark_parse_document(input.c_str(), input.size(), CMARK_OPT_DEFAULT);
        if (!node) {
            std::cerr << "Parse error\n";
            node = nullptr;
        }
    }

    ~NParser()
    {
        if (node) {
            cmark_node_free(node);
        }
    }

// Интервал строк, если доступны
void show_lines_interval(cmark_node* node) {
    int start_line = cmark_node_get_start_line(node);
    int end_line = cmark_node_get_end_line(node);
    if (start_line || end_line) {
        std::cout << " [" << start_line << "-" << end_line << "] ";
    }
}

// Содержимое текстовых узлов, code, html_inline и т.д.
void show_literal(cmark_node* node) {
    const char* lit = cmark_node_get_literal(node);
    if (lit && *lit) {
        std::cout << lit << "\n";
    }
}
void nt_none() {
    std::cout << "ERROR: Not found node\n";
}
void nt_document() {
    show_lines_interval(node);
    std::cout << "Document\n";
}
void nt_block_quote() {
    show_lines_interval(node);
    std::cout << "Block quote\n";
}
void nt_list() {
    show_lines_interval(node);
    std::cout << "List\n";
}
void nt_item() {
    show_lines_interval(node);
    std::cout << "Item\n";
}
void nt_code_block() {
    show_lines_interval(node);
    std::cout << "Code block\n";
}
void nt_html_block() {
    show_lines_interval(node);
    std::cout << "HTML block\n";
}
void nt_custom_block() {
    show_lines_interval(node);
    std::cout << "Custom block\n";
}
void nt_paragraph() {
    show_lines_interval(node);
    std::cout << "Paragraph\n";
}
void nt_heading() {
    show_lines_interval(node);
    std::cout << "Heading_";
    int level = cmark_node_get_heading_level(node);
    std::cout << level << ": ";
    
    // Текст заголовка — в первой дочерней текстовой ноде
    node = cmark_node_first_child(node);
    show_literal(node);
}
void nt_thematic_break() {
    show_lines_interval(node);
    std::cout << "Thematic break\n";
}
void nt_text() {
    show_lines_interval(node);
    std::cout << "Text: ";
    show_literal(node);
}
void nt_softbreak() {
    show_lines_interval(node);
    std::cout << " [ - ] Paragraph\n";
}
void nt_linebreak() {
    show_lines_interval(node);
    std::cout << "Linebreak\n";
}
void nt_code() {
    show_lines_interval(node);
    std::cout << "Code\n";
}
void nt_html_inline() {
    show_lines_interval(node);
    std::cout << "HTML inline\n";
}
void nt_custom_inline() {
    show_lines_interval(node);
    std::cout << "Custom inline\n";
}
void nt_emph() {
    show_lines_interval(node);
    std::cout << "Emph: ";
    node = cmark_node_first_child(node);
    show_literal(node);
}
void nt_strong() {
    show_lines_interval(node);
    std::cout << "Strong: ";
    node = cmark_node_first_child(node);
    show_literal(node);
}
void nt_link() {
    show_lines_interval(node);
    std::cout << "Link\n";
    const char* url = cmark_node_get_url(node);
    if (url && *url) {
        std::cout << "URL: " << url << "\n";
    }
    const char* title = cmark_node_get_title(node);
    if (title && *title) {
        std::cout << "Title: " << title << "\n";
    }
}
void nt_image() {
    show_lines_interval(node);
    std::cout << "Image\n";
}
void nt_unknown() {
    show_lines_interval(node);
    std::cout << "Unknown\n";
}
void dump_node()
{
  if (!node) return;
  cmark_node_type t = cmark_node_get_type(node);
    
  switch (t) {
  case CMARK_NODE_NONE:
    nt_none();
    break;
  case CMARK_NODE_DOCUMENT:
    nt_document();
    break;
  case CMARK_NODE_BLOCK_QUOTE:
    nt_block_quote();
    break;
  case CMARK_NODE_LIST:
    nt_list();
    break;
  case CMARK_NODE_ITEM:
    nt_item();
    break;
  case CMARK_NODE_CODE_BLOCK:
    nt_code_block();
    break;
  case CMARK_NODE_HTML_BLOCK:
    nt_html_block();
    break;
  case CMARK_NODE_CUSTOM_BLOCK:
    nt_custom_block();
    break;
  case CMARK_NODE_PARAGRAPH:
    nt_paragraph();
    break;
  case CMARK_NODE_HEADING:
    nt_heading();
    break;
  case CMARK_NODE_THEMATIC_BREAK:
    nt_thematic_break();
    break;
  case CMARK_NODE_TEXT:
    nt_text();
    break;
  case CMARK_NODE_SOFTBREAK:
    nt_softbreak();
    break;
  case CMARK_NODE_LINEBREAK:
    nt_linebreak();
    break;
  case CMARK_NODE_CODE:
    nt_code();
    break;
  case CMARK_NODE_HTML_INLINE:
    nt_html_inline();
    break;
  case CMARK_NODE_CUSTOM_INLINE:
    nt_custom_inline();
    break;
  case CMARK_NODE_EMPH:
    nt_emph();
    break;
  case CMARK_NODE_STRONG:
    nt_strong();
    break;
  case CMARK_NODE_LINK:
    nt_link();
    break;
  case CMARK_NODE_IMAGE:
    nt_image();
    break;
  default:
    nt_unknown();
    break;
  }

    // рекурсивно для первой дочерней, затем для последующих
    cmark_node* child = cmark_node_first_child(node);
    while (child) {
        node = child;
        dump_node();
        child = cmark_node_next(child);
    }
}

};


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: markdown_dump <file.md>\n";
        return 1;
    }

    // читаем файл полностью
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    std::string input = ss.str();

    NParser parser(input);
    // дамп начиная с корня — рекурсивная функция единственная для обхода
    parser.dump_node();
    return 0;
}
