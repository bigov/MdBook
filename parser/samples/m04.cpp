#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
extern "C" {
#include <cmark.h>
}

void dump_node(cmark_node* node)
{
  if (!node) return;
  std::cout << "type: " << cmark_node_get_type_string(node) << "\n";
  std::cout << "lit: " << cmark_node_get_literal(node) << "\n";
  std::cout << "start_line: " << cmark_node_get_start_line(node) << "\n";
  std::cout << "end_line: " << cmark_node_get_end_line(node) << "\n";

  cmark_node* child = cmark_node_first_child(node);
  while (child) {
      dump_node(child);
      child = cmark_node_next(child);
  }
}

int main(int argc, char** argv)
{
  if (argc != 2) { std::cerr << "Usage: markdown_dump <file.md>\n"; return 1; }
  std::ifstream in(argv[1], std::ios::binary);
  if (!in) { std::cerr << "Cannot open file\n"; return 1; }
  std::ostringstream ss; ss << in.rdbuf();
  std::string input = ss.str();

  cmark_node* doc = cmark_parse_document(input.c_str(), input.size(), CMARK_OPT_DEFAULT);
  if (!doc) { std::cerr << "Parse error\n"; return 1; }

  int prev_end_offset = 0;
  dump_node(doc);

  cmark_node_free(doc);
  return 0;
}
