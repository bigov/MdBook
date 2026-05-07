#include "md_parser.h"
#include <cstdlib>
#include <ostream>
#include "cmark.h"
#include "tpls.h"

namespace {

// Заменяет все вхождения строки-заполнителя placeholder в шаблоне tpl
// на строку content и возвращает результат.
//
// Особенности реализации:
// - tpl передаётся по значению намеренно: оригинальный шаблон не изменяется,
//   а копия используется как рабочий буфер — это позволяет повторно вызывать
//   функцию с одним и тем же шаблоном без побочных эффектов.
// - Если placeholder пуст, std::string::find находит совпадение в каждой
//   позиции, что привело бы к бесконечному циклу. Поэтому функция возвращает
//   шаблон без изменений.
// - Сложность: O(N * K), где N — длина tpl, K — число замен.
std::string replace_placeholder(std::string tpl, const std::string& placeholder, const std::string& content) {
    if (placeholder.empty()) {
        return tpl;
    }

    size_t pos = 0;
    while ((pos = tpl.find(placeholder, pos)) != std::string::npos) {
        tpl.replace(pos, placeholder.size(), content);
        pos += content.size();
    }
    return tpl;
}

} // namespace

// Возвращает текстовое содержимое произвольного узла AST cmark в виде
// строки UTF-8, сформированной через рендер CommonMark.
//
// Используется для узлов, где нужен «сырой» текст без XML-разметки
// (например, для отладки или для узлов, тип которых не требует стилей).
// CMARK_OPT_SMART включает типографские замены (-- → –, ... → … и т.д.).
// Завершающие символы новой строки, которые cmark добавляет в конец,
// отсекаются, чтобы не вносить лишних отступов при вставке в XML-шаблон.
std::string node_text(cmark_node *node)
{
    char *s = cmark_render_commonmark(node, CMARK_OPT_SMART, 0);
    std::string res = s ? std::string(s) : std::string();
    free(s);

    while (!res.empty() && (res.back() == '\n' || res.back() == '\r')) {
        res.pop_back();
    }
    return res;
}


// Рекурсивно собирает все литеральные фрагменты поддерева AST в одну строку.
//
// Обход: сначала литерал текущего узла, затем рекурсивный обход детей,
// затем следующий сосед (итерация по siblings идёт в цикле for).
//
// Применяется для заголовков: содержимое узла CMARK_NODE_HEADING может
// включать вложенные узлы (например, inline code), из которых нужно
// извлечь чистый текст — без XML-разметки стилей.
static std::string collect_text(cmark_node *node) {
    std::string result;
    for (cmark_node *cur = node; cur; cur = cmark_node_next(cur)) {
        const char *lit = cmark_node_get_literal(cur);
        if (lit) result += lit;
        cmark_node *child = cmark_node_first_child(cur);
        if (child) result += collect_text(child);
    }
    return result;
}

// Предварительное объявление: render_inline_siblings вызывает render_inline_node,
// а render_inline_node вызывает render_inline_siblings — взаимная рекурсия.
static std::string render_inline_node(cmark_node *node);

// Обходит цепочку узлов-соседей (siblings), начиная с node, и для каждого
// вызывает render_inline_node, конкатенируя результаты.
//
// Вместе с render_inline_node образует пару взаимно рекурсивных функций:
//   render_inline_siblings → render_inline_node → render_inline_siblings → ...
// Рекурсия завершается, когда у узла нет дочерних элементов (листовой узел).
static std::string render_inline_siblings(cmark_node *node) {
    std::string result;
    for (cmark_node *cur = node; cur; cur = cmark_node_next(cur)) {
        result += render_inline_node(cur);
    }
    return result;
}

// Рендерит один узел inline-разметки в строку XML-фрагмента.
//
// Диспетчеризация по типу узла:
//
//   CMARK_NODE_CODE   — инлайн-код (`...`): берём литерал напрямую,
//                       вложений нет и быть не может (листовой узел cmark).
//
//   CMARK_NODE_STRONG / CMARK_NODE_EMPH — жирный/курсив:
//       - Если у узла ровно один дочерний узел противоположного типа
//         (STRONG > EMPH или EMPH > STRONG), это конструкция ***text***.
//         В этом случае применяем шаблон tpl_bi (жирный + курсив).
//         Проверка «ровно один ребёнок» (first_child && !next(first_child))
//         нужна, чтобы не ошибиться на смешанных конструкциях вроде
//         **bold *mix* end** — там несколько дочерних узлов.
//       - Иначе применяем tpl_s (strong) или tpl_i (emph) и рекурсивно
//         обрабатываем дочерние узлы через render_inline_siblings.
//
//   Прочие контейнеры — рекурсивно обходим дочерние узлы.
//   Листовые узлы (нет детей) — возвращаем литерал как есть.
static std::string render_inline_node(cmark_node *node) {
    if (!node) return {};

    const cmark_node_type t = cmark_node_get_type(node);

    if (t == CMARK_NODE_CODE) {
        // Inline code: отображаем литерал как есть, без анализа вложений.
        // cmark хранит содержимое `...` в литерале узла; дочерних узлов нет.
        const char *lit = cmark_node_get_literal(node);
        return replace_placeholder(tpl_lncode, "%CONTENT%", lit ? std::string(lit) : std::string{});
    }

    if (t == CMARK_NODE_STRONG || t == CMARK_NODE_EMPH) {
        cmark_node *first_child = cmark_node_first_child(node);
        // Detect nested bold+italic: EMPH > STRONG or STRONG > EMPH (single child).
        if (first_child && !cmark_node_next(first_child)) {
            const cmark_node_type inner_t = cmark_node_get_type(first_child);
            const bool nested = (t == CMARK_NODE_STRONG && inner_t == CMARK_NODE_EMPH)
                             || (t == CMARK_NODE_EMPH  && inner_t == CMARK_NODE_STRONG);
            if (nested) {
                std::string inner = render_inline_siblings(cmark_node_first_child(first_child));
                return replace_placeholder(tpl_bi, "%CONTENT%", inner);
            }
        }
        const std::string& tpl = (t == CMARK_NODE_STRONG) ? tpl_s : tpl_i;
        std::string inner = render_inline_siblings(first_child);
        return replace_placeholder(tpl, "%CONTENT%", inner);
    }

    cmark_node *child = cmark_node_first_child(node);
    if (child) {
        return render_inline_siblings(child);
    }
    const char *lit = cmark_node_get_literal(node);
    return lit ? std::string(lit) : std::string{};
}

// Рендерит содержимое параграфа (CMARK_NODE_PARAGRAPH) в строку XML-фрагмента
// с поддержкой всех inline-стилей: жирный, курсив, жирный+курсив, инлайн-код.
//
// Реализация сводится к вызову render_inline_siblings для первого дочернего
// узла параграфа: вся логика диспетчеризации по типам узлов инкапсулирована
// внутри render_inline_node, поэтому дублировать её здесь не нужно.
static std::string render_paragraph_with_inline_styles(cmark_node *para_node) {
    return render_inline_siblings(cmark_node_first_child(para_node));
}

// Главная функция рендеринга: обходит цепочку узлов AST cmark (siblings),
// начиная с node, и записывает XML-разметку wxRichText в поток out.
//
// Пустые строки между блоками:
//   cmark хранит для каждого узла номера начальной и конечной строк исходника.
//   Если между концом предыдущего узла (prevEndLine) и началом текущего
//   (startLine) есть зазор > 1, значит в источнике были пустые строки.
//   Для каждой такой строки вставляется пустой параграф (tpl_p с пустым
//   содержимым), чтобы визуальные отступы сохранялись в отображении.
//
// Диспетчеризация по типу блочного узла:
//   CMARK_NODE_DOCUMENT — корень AST, рекурсивно обрабатываем его детей.
//   CMARK_NODE_HEADING  — заголовок h1–h6; уровень определяет индекс в tpl_h[].
//                         Текст собирается через collect_text (без inline-стилей).
//   CMARK_NODE_PARAGRAPH — параграф с возможными inline-стилями;
//                          делегируется render_paragraph_with_inline_styles.
//   CMARK_NODE_CODE_BLOCK, BLOCK_QUOTE, LIST, HTML_BLOCK — TODO, не реализовано.
//   default — если узел является контейнером (есть дети), рекурсивно
//             обрабатываем их, чтобы не потерять содержимое неизвестных узлов.
void process_node(cmark_node *node, std::ostream &out) {
    int prevEndLine = 0;
    for (cmark_node *cur = node; cur; cur = cmark_node_next(cur)) {
        const int startLine = cmark_node_get_start_line(cur);

        // Вставляем пустые параграфы для визуального отображения пустых строк.
        if (prevEndLine > 0 && startLine > 0) {
            const int blankLines = startLine - prevEndLine - 1;
            for (int i = 0; i < blankLines; ++i) {
                out << replace_placeholder(tpl_p, "%CONTENT%", "");
            }
        }

        cmark_node_type t = cmark_node_get_type(cur);
        switch (t) {
            case CMARK_NODE_DOCUMENT:
                // Корневой узел документа: рекурсивно спускаемся к содержимому.
                process_node(cmark_node_first_child(cur), out);
                break;

            case CMARK_NODE_HEADING: {
                // Уровень 1–6 кодируется индексом 0–5 в массиве tpl_h.
                // collect_text собирает текст из всего поддерева без XML-тегов.
                const int level = cmark_node_get_heading_level(cur) - 1;
                if (level >= 0 && level < 6) {
                    out << replace_placeholder(tpl_h[level], "%CONTENT%", collect_text(cmark_node_first_child(cur)));
                }
                break;
            }

            case CMARK_NODE_PARAGRAPH: {
                // Параграф может содержать смешанные inline-узлы (текст, жирный,
                // курсив, инлайн-код). Рендеринг делегируется специализированной функции.
                std::string para_content = render_paragraph_with_inline_styles(cur);
                out << replace_placeholder(tpl_p, "%CONTENT%", para_content);
                break;
            }

            case CMARK_NODE_CODE_BLOCK: {
                // TODO: CODE_BLOCK support
                /*
                const char *info = cmark_node_get_fence_info(cur);
                const char *literal = cmark_node_get_literal(cur);
                wxString lang = info ? wxString::FromUTF8(info) : wxString();
                wxString literalText = literal ? wxString::FromUTF8(literal) : wxString();
                // сохраняем код в одну строку с \n внутри
                out << "ACT_CODE";
                if (!lang.empty()) out << "(" << WxToUtf8(lang) << ")";
                out << ":" << WxToUtf8(literalText) << "\n";
                */
                break;
            }

            case CMARK_NODE_BLOCK_QUOTE: {
                // TODO: BLOCK_QUOTE support
                /*
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
                */
                break;
            }

            case CMARK_NODE_LIST: {
                // TODO: LIST support
                /*
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
                */
                break;
            }

            case CMARK_NODE_HTML_BLOCK: {
                // TODO: HTML_BLOCK support
                /*
                const char *htmlLiteral = cmark_node_get_literal(cur);
                wxString html = htmlLiteral ? wxString::FromUTF8(htmlLiteral) : wxString();
                out << "ACT_HTML:" << WxToUtf8(html) << "\n";
                */
                break;
            }

            default:
                // Неизвестный или ещё не реализованный тип узла.
                // Если у него есть дети — рекурсивно обрабатываем их,
                // чтобы текстовое содержимое не было потеряно.
                if (cmark_node_first_child(cur))
                    process_node(cmark_node_first_child(cur), out);
                break;
        }

        // Запоминаем конечную строку обработанного узла для расчёта пустых строк
        // перед следующим узлом.
        const int endLine = cmark_node_get_end_line(cur);
        if (endLine > 0) {
            prevEndLine = endLine;
        }
    }
}