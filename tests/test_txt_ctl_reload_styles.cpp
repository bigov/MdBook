#include <fstream>
#include <iostream>
#include <string>

#include <wx/frame.h>
#include <wx/init.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/richtext/richtextxml.h>
#include <wx/sstream.h>

#include "txt_ctl.h"

#ifndef TEST_MD_RELOAD_PATH
#define TEST_MD_RELOAD_PATH "../../tests/docs-tests/2.md"
#endif

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

std::string dump_buffer_xml(TxtRich& ctl)
{
    if (!wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_XML)) {
        wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    }

    wxString xml;
    wxStringOutputStream out(&xml);
    if (!ctl.GetBuffer().SaveFile(out, wxRICHTEXT_TYPE_XML)) {
        return {};
    }
    const wxScopedCharBuffer utf8 = xml.ToUTF8();
    return utf8.data() ? std::string(utf8.data()) : std::string();
}

int run_once(TxtRich& ctl, const wxString& path)
{
    ctl.load_file(path);

    const std::string xml = dump_buffer_xml(ctl);
    if (xml.empty()) {
        std::cerr << "Empty XML dump after load\n";
        return -1;
    }

    const int bold_count = count_occurrences(xml, "fontweight=\"700\"");
    if (bold_count <= 0) {
        std::cerr << "Expected at least one bold span, got " << bold_count << "\n";
        return -1;
    }

    if (xml.find("fontweight=\"700\">During in 2016") != std::string::npos) {
        std::cerr << "Paragraph text unexpectedly rendered as bold\n";
        return -1;
    }

    if (xml.find("During in 2016") == std::string::npos) {
        std::cerr << "Paragraph marker text not found in XML output\n";
        return -1;
    }

    return bold_count;
}

} // namespace

int main()
{
    wxInitializer wx;
    if (!wx.IsOk()) {
        std::cerr << "wxWidgets initialization failed\n";
        return 1;
    }

    std::ifstream f(TEST_MD_RELOAD_PATH);
    if (!f) {
        std::cerr << "Cannot open markdown: " TEST_MD_RELOAD_PATH "\n";
        return 1;
    }

    wxFrame frame(nullptr, wxID_ANY, "test");
    TxtRich ctl(&frame);

    const wxString path = wxString::FromUTF8(TEST_MD_RELOAD_PATH);
    const int bold_count_first = run_once(ctl, path);
    if (bold_count_first < 0) {
        std::cerr << "First load validation failed\n";
        return 1;
    }

    const int bold_count_second = run_once(ctl, path);
    if (bold_count_second < 0) {
        std::cerr << "Second load validation failed\n";
        return 1;
    }

    if (bold_count_first != bold_count_second) {
        std::cerr << "Bold span count changed after reload: first=" << bold_count_first
                  << ", second=" << bold_count_second << "\n";
        return 1;
    }

    std::cout << "txt_ctl_reload_styles_test: PASS\n";
    return 0;
}
