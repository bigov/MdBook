#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <wx/init.h>

#include "cmark.h"
#include "md_parser.h"

// ---------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------

namespace {

int count_occurrences(const std::string& text, const std::string& needle)
{
    if (needle.empty()) return 0;
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

// Извлекает первое числовое значение атрибута fontpointsize из строки XML.
// Возвращает -1, если атрибут не найден.
int extract_fontpointsize(const std::string& xml)
{
    const std::string key = "fontpointsize=\"";
    const auto pos = xml.find(key);
    if (pos == std::string::npos) return -1;
    const auto val_start = pos + key.size();
    const auto val_end   = xml.find('"', val_start);
    if (val_end == std::string::npos) return -1;
    try { return std::stoi(xml.substr(val_start, val_end - val_start)); }
    catch (...) { return -1; }
}

// Parse markdown, run process_node, return XML output.
std::string parse(const char* md)
{
    cmark_node* doc = cmark_parse_document(md, std::strlen(md), CMARK_OPT_SOURCEPOS);
    if (!doc) return {};
    std::ostringstream out;
    process_node(doc, out);
    cmark_node_free(doc);
    return out.str();
}

// A single named test case.
struct TestCase {
    const char* name;
    std::function<bool()> run; // returns true on pass
};

} // namespace

// ---------------------------------------------------------------------------
// Test definitions
// ---------------------------------------------------------------------------

static std::vector<TestCase> make_tests()
{
    return {
        // ------------------------------------------------------------------
        // 1. Basic: strong present in mixed paragraph
        // ------------------------------------------------------------------
        {
            "strong_in_mixed_paragraph",
            [] {
                auto xml = parse("Prefix **strong** suffix.");
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\"\n";
                    return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 2. Basic: italic present in mixed paragraph
        // ------------------------------------------------------------------
        {
            "italic_in_mixed_paragraph",
            [] {
                auto xml = parse("Prefix *italic* suffix.");
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing fontstyle=\"93\"\n";
                    return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 3. No text duplication around styled spans
        // ------------------------------------------------------------------
        {
            "no_text_duplication",
            [] {
                auto xml = parse("Prefix *italic* middle **strong** suffix.");
                bool ok = true;
                if (count_occurrences(xml, "Prefix ") != 1) {
                    std::cerr << "  'Prefix ' duplicated\n"; ok = false;
                }
                if (count_occurrences(xml, " middle ") != 1) {
                    std::cerr << "  ' middle ' duplicated\n"; ok = false;
                }
                if (count_occurrences(xml, " suffix.") != 1) {
                    std::cerr << "  ' suffix.' duplicated\n"; ok = false;
                }
                return ok;
            }
        },

        // ------------------------------------------------------------------
        // 4. Plain text paragraph — no style attributes injected
        // ------------------------------------------------------------------
        {
            "plain_paragraph_no_styles",
            [] {
                auto xml = parse("Just plain text.");
                if (xml.find("fontweight=\"700\"") != std::string::npos) {
                    std::cerr << "  Unexpected fontweight=\"700\" in plain text\n";
                    return false;
                }
                if (xml.find("fontstyle=\"93\"") != std::string::npos) {
                    std::cerr << "  Unexpected fontstyle=\"93\" in plain text\n";
                    return false;
                }
                if (xml.find("Just plain text.") == std::string::npos) {
                    std::cerr << "  Plain text content not found\n";
                    return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 5. Bold-only paragraph: strong present, italic absent
        // ------------------------------------------------------------------
        {
            "bold_only_paragraph",
            [] {
                auto xml = parse("**only bold**");
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\"\n"; return false;
                }
                if (xml.find("fontstyle=\"93\"") != std::string::npos) {
                    std::cerr << "  Unexpected fontstyle=\"93\"\n"; return false;
                }
                if (xml.find("only bold") == std::string::npos) {
                    std::cerr << "  Bold text content not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 6. Italic-only paragraph: italic present, strong absent
        // ------------------------------------------------------------------
        {
            "italic_only_paragraph",
            [] {
                auto xml = parse("*only italic*");
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing fontstyle=\"93\"\n"; return false;
                }
                if (xml.find("fontweight=\"700\"") != std::string::npos) {
                    std::cerr << "  Unexpected fontweight=\"700\"\n"; return false;
                }
                if (xml.find("only italic") == std::string::npos) {
                    std::cerr << "  Italic text content not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 7. Multiple styled spans: counts must match
        //    "**a** and *b* and **c**" → 2 × strong, 1 × italic
        // ------------------------------------------------------------------
        {
            "multiple_styled_spans_count",
            [] {
                auto xml = parse("**a** and *b* and **c**");
                const int strong_count = count_occurrences(xml, "fontweight=\"700\"");
                const int italic_count = count_occurrences(xml, "fontstyle=\"93\"");
                if (strong_count != 2) {
                    std::cerr << "  Expected 2 strong spans, got " << strong_count << "\n";
                    return false;
                }
                if (italic_count != 1) {
                    std::cerr << "  Expected 1 italic span, got " << italic_count << "\n";
                    return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 8. Nested bold+italic (***text***):
        //    cmark emits EMPH > STRONG or STRONG > EMPH — both attributes
        //    must appear somewhere in the output.
        // ------------------------------------------------------------------
        {
            "nested_bold_italic",
            [] {
                auto xml = parse("***bold and italic***");
                bool ok = true;
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\" for nested bold+italic\n"; ok = false;
                }
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing fontstyle=\"93\" for nested bold+italic\n"; ok = false;
                }
                if (xml.find("bold and italic") == std::string::npos) {
                    std::cerr << "  Nested text content not found\n"; ok = false;
                }
                return ok;
            }
        },

        // ------------------------------------------------------------------
        // 9. Cyrillic italic — text preserved without corruption
        // ------------------------------------------------------------------
        {
            "cyrillic_italic",
            [] {
                auto xml = parse("*Кириллица*");
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing fontstyle=\"93\" for Cyrillic italic\n"; return false;
                }
                // UTF-8 bytes for "Кириллица" must be present verbatim.
                if (xml.find("\xD0\x9A\xD0\xB8\xD1\x80\xD0\xB8\xD0\xBB\xD0\xBB\xD0\xB8\xD1\x86\xD0\xB0") == std::string::npos) {
                    std::cerr << "  Cyrillic text 'Кириллица' not found in output\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 10. Cyrillic strong — text preserved without corruption
        // ------------------------------------------------------------------
        {
            "cyrillic_strong",
            [] {
                auto xml = parse("**Привет мир**");
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\" for Cyrillic strong\n"; return false;
                }
                // UTF-8 for "Привет"
                if (xml.find("\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82") == std::string::npos) {
                    std::cerr << "  Cyrillic text 'Привет' not found in output\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 11. Mixed Cyrillic: plain + bold + italic
        // ------------------------------------------------------------------
        {
            "cyrillic_mixed_styles",
            [] {
                auto xml = parse("Текст **жирный** и *курсив* конец.");
                bool ok = true;
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\"\n"; ok = false;
                }
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing fontstyle=\"93\"\n"; ok = false;
                }
                // UTF-8 for "жирный"
                if (xml.find("\xD0\xB6\xD0\xB8\xD1\x80\xD0\xBD\xD1\x8B\xD0\xB9") == std::string::npos) {
                    std::cerr << "  Cyrillic bold word not found\n"; ok = false;
                }
                // UTF-8 for "курсив"
                if (xml.find("\xD0\xBA\xD1\x83\xD1\x80\xD1\x81\xD0\xB8\xD0\xB2") == std::string::npos) {
                    std::cerr << "  Cyrillic italic word not found\n"; ok = false;
                }
                return ok;
            }
        },

        // ------------------------------------------------------------------
        // 12. Heading level 1 — жирный, размер шрифта крупнее тела (12 pt)
        // ------------------------------------------------------------------
        {
            "heading_level1",
            [] {
                auto xml = parse("# Hello");
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\" for h1\n"; return false;
                }
                const int sz = extract_fontpointsize(xml);
                if (sz <= 12) {
                    std::cerr << "  h1 fontpointsize (" << sz << ") must be > 12 (body)\n"; return false;
                }
                if (xml.find("Hello") == std::string::npos) {
                    std::cerr << "  Heading text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 13. Heading level 2 — жирный, размер шрифта крупнее тела
        // ------------------------------------------------------------------
        {
            "heading_level2",
            [] {
                auto xml = parse("## Section");
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing fontweight=\"700\" for h2\n"; return false;
                }
                const int sz = extract_fontpointsize(xml);
                if (sz <= 12) {
                    std::cerr << "  h2 fontpointsize (" << sz << ") must be > 12 (body)\n"; return false;
                }
                if (xml.find("Section") == std::string::npos) {
                    std::cerr << "  Heading text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 13b. h1 крупнее h2 — визуальная иерархия заголовков
        // ------------------------------------------------------------------
        {
            "heading_h1_larger_than_h2",
            [] {
                const int sz1 = extract_fontpointsize(parse("# H1"));
                const int sz2 = extract_fontpointsize(parse("## H2"));
                if (sz1 <= sz2) {
                    std::cerr << "  h1 size (" << sz1 << ") must be > h2 size (" << sz2 << ")\n";
                    return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 14. Two paragraphs separated by blank line
        // ------------------------------------------------------------------
        {
            "two_paragraphs_blank_line",
            [] {
                auto xml = parse("First paragraph.\n\nSecond paragraph.");
                if (count_occurrences(xml, "<paragraph>") < 2) {
                    std::cerr << "  Expected at least 2 <paragraph> elements\n"; return false;
                }
                if (xml.find("First paragraph.") == std::string::npos) {
                    std::cerr << "  First paragraph text not found\n"; return false;
                }
                if (xml.find("Second paragraph.") == std::string::npos) {
                    std::cerr << "  Second paragraph text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 15. Style only at start of paragraph (no leading plain text)
        // ------------------------------------------------------------------
        {
            "style_at_paragraph_start",
            [] {
                auto xml = parse("**Bold start** then normal.");
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing bold at paragraph start\n"; return false;
                }
                if (xml.find("Bold start") == std::string::npos) {
                    std::cerr << "  Bold text not found\n"; return false;
                }
                if (xml.find("then normal.") == std::string::npos) {
                    std::cerr << "  Trailing normal text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 16. Style only at end of paragraph (no trailing plain text)
        // ------------------------------------------------------------------
        {
            "style_at_paragraph_end",
            [] {
                auto xml = parse("Normal then *italic end*");
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing italic at paragraph end\n"; return false;
                }
                if (xml.find("italic end") == std::string::npos) {
                    std::cerr << "  Italic text not found\n"; return false;
                }
                if (xml.find("Normal then ") == std::string::npos) {
                    std::cerr << "  Leading normal text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 17. Inline code: моноширинное семейство шрифтов (fontfamily="75")
        //     fontfamily="75" — фиксированный код моноширинного семейства
        //     в wxWidgets; не зависит от конкретного имени шрифта в шаблоне.
        // ------------------------------------------------------------------
        {
            "inline_code_font",
            [] {
                auto xml = parse("Use `printf()` to print.");
                if (xml.find("fontfamily=\"75\"") == std::string::npos) {
                    std::cerr << "  Missing fontfamily=\"75\" (monospace) for inline code\n"; return false;
                }
                if (xml.find("printf()") == std::string::npos) {
                    std::cerr << "  Inline code text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 18. Inline code: surrounding plain text preserved
        // ------------------------------------------------------------------
        {
            "inline_code_surrounding_text",
            [] {
                auto xml = parse("Call `foo()` here.");
                if (xml.find("Call ") == std::string::npos) {
                    std::cerr << "  Leading plain text not found\n"; return false;
                }
                if (xml.find("here.") == std::string::npos) {
                    std::cerr << "  Trailing plain text not found\n"; return false;
                }
                if (xml.find("foo()") == std::string::npos) {
                    std::cerr << "  Code text not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 19. Inline code: span оформлен как styled-элемент
        //     Проверяем, что код находится внутри <text> с явным атрибутом
        //     textcolor (т.е. шаблон применяет отдельный styled-тег).
        //     Конкретный цвет — настройка дизайна, здесь не проверяется.
        // ------------------------------------------------------------------
        {
            "inline_code_is_styled",
            [] {
                auto xml = parse("Result: `42`.");
                // Styled code span открывается тегом <text textcolor=
                if (xml.find("<text textcolor=") == std::string::npos) {
                    std::cerr << "  Inline code not rendered as styled <text> element\n"; return false;
                }
                if (xml.find("42") == std::string::npos) {
                    std::cerr << "  Code text '42' not found\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 20. Inline code: no bold/italic styling leaked
        // ------------------------------------------------------------------
        {
            "inline_code_no_bold_italic_leak",
            [] {
                auto xml = parse("Run `cmd`.");
                // The inline code span must not carry fontweight=700 or fontstyle=93.
                // We check by ensuring only the default-paragraph fontweight appears,
                // not the strong one. The default paragraphlayout has fontweight=400.
                if (xml.find("fontweight=\"700\"") != std::string::npos) {
                    std::cerr << "  Unexpected fontweight=700 in code-only paragraph\n"; return false;
                }
                if (xml.find("fontstyle=\"93\"") != std::string::npos) {
                    std::cerr << "  Unexpected fontstyle=93 in code-only paragraph\n"; return false;
                }
                return true;
            }
        },

        // ------------------------------------------------------------------
        // 21. Mixed: bold, italic, and inline code in same paragraph
        // ------------------------------------------------------------------
        {
            "mixed_bold_italic_code",
            [] {
                auto xml = parse("**bold**, *italic*, and `code`.");
                bool ok = true;
                if (xml.find("fontweight=\"700\"") == std::string::npos) {
                    std::cerr << "  Missing bold\n"; ok = false;
                }
                if (xml.find("fontstyle=\"93\"") == std::string::npos) {
                    std::cerr << "  Missing italic\n"; ok = false;
                }
                if (xml.find("fontfamily=\"75\"") == std::string::npos) {
                    std::cerr << "  Missing monospace family (fontfamily=75) for code\n"; ok = false;
                }
                if (xml.find("bold") == std::string::npos || xml.find("italic") == std::string::npos || xml.find("code") == std::string::npos) {
                    std::cerr << "  Some text content missing\n"; ok = false;
                }
                return ok;
            }
        },
    };
}

// ---------------------------------------------------------------------------
// Runner
// ---------------------------------------------------------------------------

int main()
{
    wxInitializer wx;

    auto tests = make_tests();
    int failed = 0;

    for (auto& tc : tests) {
        const bool ok = tc.run();
        if (ok) {
            std::cout << "  PASS  " << tc.name << "\n";
        } else {
            std::cerr << "  FAIL  " << tc.name << "\n";
            ++failed;
        }
    }

    std::cout << "\n" << (tests.size() - failed) << "/" << tests.size() << " tests passed";
    if (failed == 0) {
        std::cout << " — all OK\n";
        return 0;
    }
    std::cout << "\n";
    return 1;
}
