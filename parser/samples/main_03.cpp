#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
extern "C" {
#include <cmark.h>
}

// helper: получить offset начала узла; вернёт -1 если недоступно
static int get_node_start_offset(cmark_node* n) {
#if defined(CMARK_NODE_HAS_POSITION) || defined(CMARK_NODE_START_OFFSET)
    // если cmark предоставляет функцию cmark_node_get_start_offset
    return cmark_node_get_start_offset(n);
#else
    (void)n;
    return -1;
#endif
}
static int get_node_end_offset(cmark_node* n) {
#if defined(CMARK_NODE_HAS_POSITION) || defined(CMARK_NODE_END_OFFSET)
    return cmark_node_get_end_offset(n);
#else
    (void)n;
    return -1;
#endif
}

// fallback: вычисляем смещение в input по номеру строки (1-based)
static int line_to_offset(const std::string &input, int line_number) {
    if (line_number <= 1) return 0;
    int ln = 1;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\n') {
            ++ln;
            if (ln == line_number) return (int)(i + 1);
        }
    }
    // если запрошенная строка за концом — вернуть размер
    return (int)input.size();
}

void dump_node_recursive(cmark_node* node, int depth, const std::string &input, int &prev_end_offset) {
    if (!node) return;

    const char* type = cmark_node_get_type_string(node);
    int start_line = cmark_node_get_start_line(node);
    int end_line = cmark_node_get_end_line(node);
    int start_offset = get_node_start_offset(node);
    int end_offset = get_node_end_offset(node);

    // fallback на вычисление по номеру строки, если offset-функции отсутствуют/вернули -1
    if (start_offset < 0 && start_line > 0) start_offset = line_to_offset(input, start_line);
    if (end_offset < 0 && end_line > 0) {
        // end_offset должен указывать на позицию после конца; для простоты ставим начало следующей строки или конец файла
        if (end_line + 1 > 0) end_offset = line_to_offset(input, end_line + 1);
        if (end_offset == 0) end_offset = (int)input.size();
    }

    // посчитать количество '\n' между prev_end_offset и start_offset
    if (start_offset > prev_end_offset) {
        int nl = 0;
        for (int i = prev_end_offset; i < start_offset && i < (int)input.size(); ++i) {
            if (input[i] == '\n') ++nl;
        }
        if (nl > 0) {
            for (int i = 0; i < depth; ++i) std::cout << "  ";
            std::cout << "[newlines: " << nl << "]\n";
        }
    }

    for (int i = 0; i < depth; ++i) std::cout << "  ";
    std::cout << type;
    if (start_line || end_line) std::cout << " [" << start_line << "-" << end_line << "]";
    const char* lit = cmark_node_get_literal(node);
    if (lit && *lit) std::cout << " : \"" << lit << "\"";
    std::cout << "\n";

    // рекурсивный обход детей
    cmark_node* child = cmark_node_first_child(node);
    while (child) {
        dump_node_recursive(child, depth + 1, input, prev_end_offset);
        child = cmark_node_next(child);
    }

    // обновляем prev_end_offset после обработки узла (берём end_offset если есть)
    if (end_offset > 0) prev_end_offset = end_offset;
    else if (end_line > 0) prev_end_offset = line_to_offset(input, end_line + 1);
}

int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "Usage: markdown_dump <file.md>\n"; return 1; }
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) { std::cerr << "Cannot open file\n"; return 1; }
    std::ostringstream ss; ss << in.rdbuf();
    std::string input = ss.str();

    cmark_node* doc = cmark_parse_document(input.c_str(), input.size(), CMARK_OPT_DEFAULT);
    if (!doc) { std::cerr << "Parse error\n"; return 1; }

    int prev_end_offset = 0;
    dump_node_recursive(doc, 0, input, prev_end_offset);

    cmark_node_free(doc);
    return 0;
}
