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
    int row;
    void next_row(cmark_node* n);
    void blank_row(int count);
    void show_lines_interval(cmark_node* n);
    void show_literal(cmark_node* n);
    void nt_none();
    void nt_document(cmark_node* n);
    void nt_block_quote(cmark_node* n);
    void nt_list(cmark_node* n);
    void nt_item(cmark_node* n);
    void nt_code_block(cmark_node* n);
    void nt_html_block(cmark_node* n);
    void nt_custom_block(cmark_node* n);
    void nt_paragraph(cmark_node* n);
    void nt_heading(cmark_node* n);
    void nt_thematic_break(cmark_node* n);
    void nt_text(cmark_node* n);
    void nt_softbreak(cmark_node* n);
    void nt_linebreak(cmark_node* n);
    void nt_code(cmark_node* n);
    void nt_html_inline(cmark_node* n);
    void nt_custom_inline(cmark_node* n);
    void nt_emph(cmark_node* n);
    void nt_strong(cmark_node* n);
    void nt_link(cmark_node* n);
    void nt_image(cmark_node* n);
    void nt_unknown(cmark_node* n);

public:
    NParser(const std::string& input);
    ~NParser();
    void dump_nodes_tree(cmark_node* n);
};

NParser::NParser(const std::string& input)
{
    row = 0;
    node = cmark_parse_document(input.c_str(), input.size(), CMARK_OPT_DEFAULT|CMARK_OPT_VALIDATE_UTF8);
    if (!node) {
        std::cerr << "Parse error\n";
        node = nullptr;
    }
}

NParser::~NParser()
{
    if (node) {
        cmark_node_free(node);
    }
}

void NParser::next_row(cmark_node* n) {
    this->row++;
    std::cout << this->row << ": ";

    int s = cmark_node_get_start_line(n);
    if(s > this->row) blank_row(s - this->row);
}

void NParser::blank_row(int count) {
    for (int i = 0; i < count; i++) {
        std::cout << "--- --- ---\n";
        next_row(NULL);
    }
}

// Интервал строк, если доступны
void NParser::show_lines_interval(cmark_node* n) {
    return;

    int s = cmark_node_get_start_line(n);
    int e = cmark_node_get_end_line(n);
    if(cmark_node_is_block(n)) {
      std::cout << "[" << s << "-" << e << "] ";
    }
    if(cmark_node_is_inline(n)) {
      std::cout << "[ " << s << " ] ";
    }
}

// Содержимое текстовых узлов, code, html_inline и т.д.
void NParser::show_literal(cmark_node* n) {
    const char* lit = cmark_node_get_literal(n);
    if (lit && *lit) {
        std::cout << lit << "\n";
    }
}
void NParser::nt_none() {
    std::cout << "ERROR: Not found node\n";
}
void NParser::nt_document(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Document\n";
}
void NParser::nt_block_quote(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Block quote\n";
}
void NParser::nt_list(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "List\n";
}
void NParser::nt_item(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Item\n";
}
void NParser::nt_code_block(cmark_node* n) {
  next_row(n);
  show_lines_interval(n);
  std::cout << "``` START code block";
  const char* info = cmark_node_get_fence_info(n); // язык/инфо
  if (info && *info) std::cout << " \"" << info << "\"";
  std::cout << "\n";
  const char* lit = cmark_node_get_literal(n);
  if (lit && *lit) {
    // выводим каждую строку тела с дополнительным отступом
    std::istringstream ss(lit);
    std::string line;
    while (std::getline(ss, line)) {
      next_row(n);
      std::cout << line << "\n";
    }
    next_row(n);
    std::cout << "``` End code block.\n";
  }
}
void NParser::nt_html_block(cmark_node* n) {
    next_row(n);
    show_lines_interval(n);
    std::cout << "HTML block\n";
}
void NParser::nt_custom_block(cmark_node* n) {
    next_row(n);
    show_lines_interval(n);
    std::cout << "Custom block\n";
}
void NParser::nt_paragraph(cmark_node* n) {
    next_row(n);
    show_lines_interval(n);
    std::cout << "Paragraph: ";
}
void NParser::nt_heading(cmark_node* n) {
    next_row(n);
    show_lines_interval(n);
    std::cout << "Heading_";
    int level = cmark_node_get_heading_level(n);
    std::cout << level << ": ";

    // Текст заголовка — в первой дочерней текстовой ноде
    //cmark_node* d = cmark_node_first_child(n);
    //show_literal(d);
}
void NParser::nt_thematic_break(cmark_node* n) {
    next_row(n);
    show_lines_interval(n);
    std::cout << "Thematic break\n";
}
void NParser::nt_text(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Text: ";
    show_literal(n);
}
void NParser::nt_softbreak(cmark_node* n) {
    next_row(n);
    // У softbreak нет номеров строк
    std::cout << "Softbreak: ";
}
void NParser::nt_linebreak(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Linebreak\n";
}
void NParser::nt_code(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "CodeL: " << cmark_node_get_literal(n) << "\n";
}
void NParser::nt_html_inline(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "HTML inline\n";
}
void NParser::nt_custom_inline(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Custom inline\n";
}
void NParser::nt_emph(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Emph: ";
    //n = cmark_node_first_child(n);
    //show_literal(n);
}
void NParser::nt_strong(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Strong: ";
    //n = cmark_node_first_child(n);
    //show_literal(n);
}
void NParser::nt_link(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Link\n";
    const char* url = cmark_node_get_url(n);
    if (url && *url) {
        std::cout << "URL: " << url << "\n";
    }
    const char* title = cmark_node_get_title(n);
    if (title && *title) {
        std::cout << "Title: " << title << "\n";
    }
}
void NParser::nt_image(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Image\n";
}
void NParser::nt_unknown(cmark_node* n) {
    show_lines_interval(n);
    std::cout << "Unknown\n";
}

void NParser::dump_nodes_tree(cmark_node* n = NULL)
{
  if (n == NULL) n = this->node;
  
  cmark_node_type t = cmark_node_get_type(n);
    
  switch (t) {
  case CMARK_NODE_NONE:
    nt_none();
    break;
  // -- Block nodes --
  case CMARK_NODE_DOCUMENT:
    nt_document(n);
    break;
  case CMARK_NODE_BLOCK_QUOTE:
    nt_block_quote(n);
    break;
  case CMARK_NODE_LIST:
    nt_list(n);
    break;
  case CMARK_NODE_ITEM:
    nt_item(n);
    break;
  case CMARK_NODE_CODE_BLOCK:
    nt_code_block(n);
    break;
  case CMARK_NODE_HTML_BLOCK:
    nt_html_block(n);
    break;
  case CMARK_NODE_CUSTOM_BLOCK:
    nt_custom_block(n);
    break;
  case CMARK_NODE_PARAGRAPH:
    nt_paragraph(n);
    break;
  case CMARK_NODE_HEADING:
    nt_heading(n);
    break;
  case CMARK_NODE_THEMATIC_BREAK:
    nt_thematic_break(n);
    break;
  // -- Inline nodes --
  case CMARK_NODE_TEXT:
    nt_text(n);
    break;
  case CMARK_NODE_SOFTBREAK:
    nt_softbreak(n);
    break;
  case CMARK_NODE_LINEBREAK:
    nt_linebreak(n);
    break;
  case CMARK_NODE_CODE:
    nt_code(n);
    break;
  case CMARK_NODE_HTML_INLINE:
    nt_html_inline(n);
    break;
  case CMARK_NODE_CUSTOM_INLINE:
    nt_custom_inline(n);
    break;
  case CMARK_NODE_EMPH:
    nt_emph(n);
    break;
  case CMARK_NODE_STRONG:
    nt_strong(n);
    break;
  case CMARK_NODE_LINK:
    nt_link(n);
    break;
  case CMARK_NODE_IMAGE:
    nt_image(n);
    break;
  default:
    nt_unknown(n);
    break;
  }

    // Рекурсивный обход
    cmark_node* child = cmark_node_first_child(n);
    while (child) {
        cmark_node* n = child;
        dump_nodes_tree(n);
        child = cmark_node_next(child);
    }
}


/*
// There are helper functions to determine node characteristics:
 bool cmark_node_is_block(cmark_node *node);
 bool cmark_node_is_inline(cmark_node *node);
 bool cmark_node_is_leaf(cmark_node *node); // Cannot contain children

 // по 'width' можно нормализовать текст
  int options = CMARK_OPT_DEFAULT;
  int width = 78;
  char *xml = cmark_render_commonmark(node, options, width);
  printf("%s", xml);
  return;
*/


int main(int argc, char** argv) {
    std::cout << "\n";
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
    printf("%s\n\n", input.c_str());

    NParser parser(input.c_str());
    parser.dump_nodes_tree();
    return 0;
}
