#include <iosfwd>
#include <string>

struct cmark_node;

// md_parser module purpose:
// - Traverse cmark AST produced from Markdown input.
// - Build wxRichText XML fragments using string templates from tpls.h.
// - Preserve paragraph boundaries and basic inline styling (currently strong).
//
// process_node:
// - Input: pointer to first cmark node in a subtree.
// - Output: serialized XML fragments written to out.
// - Responsibility: block-level dispatch (document, heading, paragraph, etc.)
//   and recursive traversal of children.
//
// node_text:
// - Utility for plain textual extraction via cmark rendering.
// - Trims trailing CR/LF to keep template output stable.

void process_node(cmark_node *node, std::ostream &out);
std::string node_text(cmark_node *node);
