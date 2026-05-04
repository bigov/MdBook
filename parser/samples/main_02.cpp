#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
extern "C" {
#include <cmark.h>
}

void dump_node_recursive(cmark_node* node, int depth, int &prev_end_line) {
    if (!node) return;

    const char* type = cmark_node_get_type_string(node);
    int start_line = cmark_node_get_start_line(node);
    int end_line = cmark_node_get_end_line(node);
    const char* lit = cmark_node_get_literal(node);

    // Если этот узел — параграф, считаем пустые строки между ним и предыдущим узлом
    if (strcmp(type, "paragraph") == 0 && start_line > 0 && prev_end_line > 0) {
        int blank_lines = start_line - prev_end_line - 1;
        if (blank_lines > 0) {
            for (int i = 0; i < depth; ++i) std::cout << "  ";
            std::cout << "[blank lines: " << blank_lines << "]\n";
        }
    }

    for (int i = 0; i < depth; ++i) std::cout << "  ";
    std::cout << type;
    if (start_line || end_line) std::cout << " [" << start_line << "-" << end_line << "]";
    if (lit && *lit) std::cout << " : \"" << lit << "\"";
    std::cout << "\n";

    // Рекурсивно обход детей (единственная рекурсивная функция)
    cmark_node* child = cmark_node_first_child(node);
    while (child) {
        dump_node_recursive(child, depth + 1, prev_end_line);
        child = cmark_node_next(child);
    }

    // Обновляем prev_end_line после обработки этого узла (важно для подсчёта пустых строк следующего блока)
    if (end_line > 0) prev_end_line = end_line;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: markdown_dump <file.md>\n";
        return 1;
    }
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) { std::cerr << "Cannot open file\n"; return 1; }
    std::ostringstream ss; ss << in.rdbuf();
    std::string input = ss.str();

    cmark_node* doc = cmark_parse_document(input.c_str(), input.size(), CMARK_OPT_DEFAULT);
    if (!doc) { std::cerr << "Parse error\n"; return 1; }

    int prev_end_line = 0;
    dump_node_recursive(doc, 0, prev_end_line);

    cmark_node_free(doc);
    return 0;
}
