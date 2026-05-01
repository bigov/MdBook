#include "md_parser.h"
#include <ostream>
#include "wx/string.h"
#include "cmark.h"

namespace {

std::string WxToUtf8(const wxString& value) {
    const wxScopedCharBuffer utf8 = value.ToUTF8();
    return utf8.data() ? std::string(utf8.data()) : std::string();
}

} // namespace

std::string node_text(cmark_node *node) {
    // Собираем текстовое представление узла (рекурсивно для inline-узлов)
    char *s = cmark_render_commonmark(node, CMARK_OPT_SMART, 0);
    wxString res = s ? wxString::FromUTF8(s) : wxString();
    free(s);
    // Удалим возможные завершающие новые строки
    while (!res.empty() && (res.Last() == '\n' || res.Last() == '\r')) {
        res.RemoveLast();
    }
    return WxToUtf8(res);
}

void process_node(cmark_node *node, std::ostream &out) {
    for (cmark_node *cur = node; cur; cur = cmark_node_next(cur)) {
        cmark_node_type t = cmark_node_get_type(cur);
        switch (t) {
            case CMARK_NODE_DOCUMENT:
                process_node(cmark_node_first_child(cur), out);
                break;

            case CMARK_NODE_HEADING: {
                int level = cmark_node_get_heading_level(cur);
                std::string text = node_text(cur);
                out << "ACT_HEADING(" << level << "):" << text << "\n";
                break;
            }

            case CMARK_NODE_PARAGRAPH: {
                std::string text = node_text(cur);
                out << "ACT_PAR:" << text << "\n";
                break;
            }

            case CMARK_NODE_CODE_BLOCK: {
                const char *info = cmark_node_get_fence_info(cur);
                const char *literal = cmark_node_get_literal(cur);
                wxString lang = info ? wxString::FromUTF8(info) : wxString();
                wxString literalText = literal ? wxString::FromUTF8(literal) : wxString();
                // сохраняем код в одну строку с \n внутри
                out << "ACT_CODE";
                if (!lang.empty()) out << "(" << WxToUtf8(lang) << ")";
                out << ":" << WxToUtf8(literalText) << "\n";
                break;
            }

            case CMARK_NODE_BLOCK_QUOTE: {
                // Блок-цитата может содержать несколько детей — обрабатываем их как цитаты (склеиваем)
                // Для простоты выводим каждую внутреннюю параграфную строку как ACT_QUOTE
                for (cmark_node *child = cmark_node_first_child(cur); child; child = cmark_node_next(child)) {
                    if (cmark_node_get_type(child) == CMARK_NODE_PARAGRAPH) {
                        std::string q = node_text(child);
                        out << "ACT_QUOTE:" << q << "\n";
                    } else {
                        process_node(child, out);
                    }
                }
                break;
            }

            case CMARK_NODE_LIST: {
                // Проходим элементы списка
                for (cmark_node *item = cmark_node_first_child(cur); item; item = cmark_node_next(item)) {
                    // Каждый item содержит одного или нескольких детей (обычно параграфы)
                    for (cmark_node *child = cmark_node_first_child(item); child; child = cmark_node_next(child)) {
                        // Для простоты: все параграфы/inline внутри элемента считаем одним строковым элементом
                        if (cmark_node_get_type(child) == CMARK_NODE_PARAGRAPH) {
                            std::string it = node_text(child);
                            out << "ACT_LISTITEM:" << it << "\n";
                        } else {
                            process_node(child, out);
                        }
                    }
                }
                break;
            }

            case CMARK_NODE_HTML_BLOCK: {
                const char *htmlLiteral = cmark_node_get_literal(cur);
                wxString html = htmlLiteral ? wxString::FromUTF8(htmlLiteral) : wxString();
                out << "ACT_HTML:" << WxToUtf8(html) << "\n";
                break;
            }

            // Пропускаем заголовки уровня inline (текст, emphasis, strong, link, code) — они уже включены в node_text()
            default:
                // если узел содержит детей, рекурсивно обработаем их
                if (cmark_node_first_child(cur))
                    process_node(cmark_node_first_child(cur), out);
                break;
        }
    }
}