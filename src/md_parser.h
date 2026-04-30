#include <iosfwd>
#include <string>

struct cmark_node;

void process_node(cmark_node *node, std::ostream &out);
std::string node_text(cmark_node *node);
