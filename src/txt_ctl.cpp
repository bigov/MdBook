#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <cstring>

#include "wx/menu.h"
#include "wx/richtext/richtextxml.h"
#include "wx/wfstream.h"
#include "wx/sstream.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/textdlg.h"
#include "wx/tokenzr.h"
#include "wx/log.h"
#include "txt_ctl.h"


namespace
{
    std::string to_utf8(const wxString& value)
    {
        const wxScopedCharBuffer utf8 = value.ToUTF8();
        return utf8.data() ? std::string(utf8.data()) : std::string();
    }

    std::string replace_placeholder(std::string tpl, const std::string& placeholder, const std::string& content)
    {
        size_t pos = 0;
        while ((pos = tpl.find(placeholder, pos)) != std::string::npos)
        {
            tpl.replace(pos, placeholder.size(), content);
            pos += content.size();
        }
        return tpl;
    }

    bool isFileExist(const wxString filePath)
    {
        if (wxFileExists(filePath)) return true;
        wxLogError(_("Not found file '%s'."), filePath.wc_str());
        return false;
    }

    void load_file_content(const wxString& filePath, wxString& content)
    {
        if (!isFileExist(filePath)) return;
        wxFileInputStream input_stream(filePath);
        if (!input_stream.IsOk())
        {
            wxLogError(_("Cannot open file '%s'."), filePath.wc_str());
            return;
        }

        wxStringOutputStream string_stream(&content);
        input_stream.Read(string_stream);

        if (input_stream.GetLastError() != wxSTREAM_NO_ERROR &&
            input_stream.GetLastError() != wxSTREAM_EOF)
        {
            wxLogError(_("Cannot read file '%s'."), filePath.wc_str());
            return;
        }

        // Normalize line breaks to Unix style for consistent processing
        content.Replace("\r\n", "\n");
        content.Replace("\r", "\n");
    }
} // namespace

// Конструктор класса TxtCtl
TxtCtl::TxtCtl(wxWindow* parent)
    : wxRichTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                    wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS)
{
    m_styleSheet = std::make_unique<wxRichTextStyleSheet>();
    node_current = nullptr;

    this->style_base.SetFlags(wxTEXT_ATTR_FONT | wxTEXT_ATTR_TEXT_COLOUR
         | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_ALIGNMENT);
    this->style_base.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    this->style_urls.SetFlags(wxTEXT_ATTR_FONT
         | wxTEXT_ATTR_TEXT_COLOUR
         | wxTEXT_ATTR_BACKGROUND_COLOUR);

    wxFont base_font = wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxColor color_base_fg = "#444444";
    wxColor color_urls_fg = "#25A4D1"; 
    wxColor color_base_bg = "#ffffff";

    this->style_base.SetFont(base_font);
    this->style_base.SetTextColour(color_base_fg);
    this->style_base.SetBackgroundColour(color_base_bg);
    this->style_base.SetFontWeight(wxFONTWEIGHT_NORMAL);

    this->style_urls.SetTextColour(color_urls_fg);
    this->style_urls.SetFontUnderlined(true);
    auto linkStyle = std::make_unique<wxRichTextCharacterStyleDefinition>("style_urls");
    linkStyle->SetStyle(this->style_urls);
    this->m_styleSheet->AddCharacterStyle(linkStyle.get());
    linkStyle.release();
    
    this->SetStyleSheet(this->m_styleSheet.get());

    this->node_current = nullptr;
    new_document();
}

void TxtCtl::new_document()
{
    this->Clear();
    this->SetDefaultStyle(this->style_base);
    this->SetBasicStyle(this->style_base);
    this->GetBuffer().SetDefaultStyle(this->style_base);
    this->SetAndShowDefaultStyle(this->style_base);
    this->SetMargins(6, 4);
    this->SetInsertionPoint(0);
    this->row_current = 0;
    this->row_total = 0;
}

void TxtCtl::load_xml_handler()
{
    if (!wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_XML))
    {
        wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    }
}

// --- Save the buffer content as plain text ---
void TxtCtl::save_plain_file(const wxString filePath)
{
    wxRichTextBuffer& buffer = this->GetBuffer();
    const wxString plain_text = buffer.GetText().utf8_str();
    wxFileOutputStream output_stream(filePath);
    output_stream.Write(plain_text.data(), plain_text.length());

    if (output_stream.GetLastError() != wxSTREAM_NO_ERROR)
    {
        wxLogError(_("Error while writing to file '%s'."), filePath.wc_str());
        return;
    }
}

void TxtCtl::save_xml_file(const wxString filePath)
{
    wxRichTextBuffer& buffer = this->GetBuffer();
    load_xml_handler();
    if (!buffer.SaveFile(filePath, wxRICHTEXT_TYPE_XML))
    {
        wxLogError(_("Cannot save rich buffer file '%s'."), filePath.wc_str());
    }
    return;
}

// --- Load the prepared XML data into the control's buffer ---
void TxtCtl::push_xml_data(const wxString& content)
{
    wxStringInputStream xml_stream(content);
    wxRichTextBuffer& buffer = this->GetBuffer();
    load_xml_handler();
    if (!buffer.LoadFile(xml_stream, wxRICHTEXT_TYPE_XML))
    {
        wxLogWarning(_("Cannot load XML from string."));
        SetValue(content);
        return;
    }
    this->Refresh();
}

// --- Load files with any formats ---
void TxtCtl::load_file(const wxString filePath)
{
    auto fileName = wxFileName(filePath);
    wxString fileExt = fileName.GetExt();
    fileExt.LowerCase();

    if(fileExt == RICH_BUFFER_EXT) {
        this->load_xml_file(filePath);
    } else if(fileExt == MARK_BUFFER_EXT) {
        this->load_md_file(filePath);
    } else {
        this->load_plain_file(filePath);
    }
}

// --- Load the Markdown file text ---
void TxtCtl::load_md_file(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString plain_text;
    load_file_content(filePath, plain_text);
    cmark_node* node = cmark_parse_document(plain_text.c_str(),
         plain_text.size(), CMARK_OPT_DEFAULT);
    if (!node) {
        wxLogError(_("Error parsing file '%s'."), filePath.wc_str());
        node = nullptr;
        return;
    }
    this->node_current = node;
    this->BeginSuppressUndo();
    // Defensive cleanup in case a previous render left style state in stack.
    this->EndAllStyles();
    deploy_md_node();
    this->node_current = nullptr;
    // Дополнить пустые строки до конца документа.
    while (this->row_current < this->row_total) {
        next_line();
    }
    cmark_node_free(node);
    // Ensure no transient rendering styles survive after markdown traversal.
    this->EndAllStyles();
    this->EndSuppressUndo();
    this->SetInsertionPoint(0);
    this->SetDefaultStyle(this->style_base);
    this->GetBuffer().SetDefaultStyle(this->style_base);
    this->SetAndShowDefaultStyle(this->style_base);
}

// --- Load the plain text content from a file ---
void TxtCtl::load_plain_file(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString plain_text;
    load_file_content(filePath, plain_text);

    new_document();
    this->BeginSuppressUndo();
    this->WriteText(plain_text);
    this->EndSuppressUndo();
    this->SetInsertionPoint(0);
}

// --- Load the XML content from a file ---
void TxtCtl::load_xml_file(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString xml_content;
    load_file_content(filePath, xml_content);
    push_xml_data(xml_content);
}

// Переход на следующую строку.
void TxtCtl::next_line() {
    this->row_current++;
    //В начале документа новый абзац не создавать (уже есть).
    if(this->row_current > 1) Newline();
    row_check();
}

void TxtCtl::row_check() {
    if(this->node_current == nullptr) return;
    int row_node_start = cmark_node_get_start_line(this->node_current);
    while (row_node_start > this->row_current)
    {
        this->row_current++;
        Newline();
    }
}

// Содержимое текстовых узлов, code, html_inline и т.д.
void TxtCtl::show_literal(cmark_node* n) {
    const char* lit = cmark_node_get_literal(n);
    if (lit && *lit) this->WriteText(std::string(lit));
}

void TxtCtl::md_none() {
    this->WriteText("ERROR: Not found node\n");
}

void TxtCtl::md_blockquote(cmark_node* n) {
    this->WriteText("Block quote\n");
}

void TxtCtl::md_list(cmark_node* n) {
    this->WriteText("List\n");
}

void TxtCtl::md_item(cmark_node* n) {
    this->WriteText("Item\n");
}

void TxtCtl::md_code_block() {
  this->WriteText(" --- --- --- ");
  WriteText(cmark_node_get_fence_info(this->node_current));
  this->WriteText(" --- --- --- ");
  // Многострочный литерал
  const char* lit = cmark_node_get_literal(this->node_current);
  if (lit && *lit) {
    std::istringstream ss(lit);
    std::string line;
    // Вывести построчно c подсчетом числа строк
    while (std::getline(ss, line)) {
      next_line();
      this->WriteText(line);
    }
    next_line();
    this->WriteText(" --- --- --- ");
  }
}

void TxtCtl::md_html_block(cmark_node* n) {
    this->WriteText("HTML block\n");
}
void TxtCtl::md_custom_block(cmark_node* n) {
    this->WriteText("Custom block\n");
}

void TxtCtl::md_header() {
    // Размер заголовка
    int font_size = 16 - cmark_node_get_heading_level(this->node_current);
    this->BeginFontSize(font_size);
    this->BeginBold();

    // Текст заголовка — в первой дочерней текстовой ноде
    this->node_current = cmark_node_first_child(this->node_current);
    show_literal(this->node_current);
    this->EndBold();
    this->EndFontSize();
}
void TxtCtl::md_thematic_break(cmark_node* n) {
    this->WriteText("Thematic break\n");
}
void TxtCtl::md_text(cmark_node* n) {
    show_literal(n);
}

void TxtCtl::md_code() {
    this->BeginBold();
    this->WriteText("'");
    show_literal(this->node_current);
    this->WriteText("'");
    this->EndBold();
}
void TxtCtl::md_html_inline(cmark_node* n) {
    this->WriteText("HTML inline\n");
}
void TxtCtl::md_custom_inline(cmark_node* n) {
    this->WriteText("Custom inline\n");
}
void TxtCtl::md_emph() {
    this->BeginItalic();
    this->node_current = cmark_node_first_child(this->node_current);
    show_literal(this->node_current);
    this->EndItalic();
}
void TxtCtl::md_strong(cmark_node* n) {
    this->WriteText("Strong: ");
    //n = cmark_node_first_child(n);
    //show_literal(n);
}
void TxtCtl::md_link() {
    const char *url = cmark_node_get_url(this->node_current);
    //const char *title = cmark_node_get_title(this->node_current);
    this->node_current = cmark_node_first_child(this->node_current);
    const char *text = cmark_node_get_literal(this->node_current);
    this->BeginURL(url, "style_urls");
    this->WriteText(text);
    this->EndURL();
}
void TxtCtl::md_image(cmark_node* n) {
    this->WriteText("Image\n");
}
void TxtCtl::md_unknown(cmark_node* n) {
    this->WriteText("Unknown\n");
}

// ---------------------------------------------


void TxtCtl::deploy_md_node()
{
  if (!node_current) return;
  cmark_node_type t = cmark_node_get_type(node_current);
    
  switch (t) {
  case CMARK_NODE_NONE:
    md_none();
    break;
  // -- Block nodes --
  case CMARK_NODE_DOCUMENT:
    new_document();
    this->row_total = cmark_node_get_end_line(this->node_current);
    break;
  case CMARK_NODE_HEADING:
    next_line();
    md_header();
    break;
  case CMARK_NODE_PARAGRAPH:
    next_line();
    break;
  case CMARK_NODE_BLOCK_QUOTE:
    next_line();
    md_blockquote(node_current);
    break;
  case CMARK_NODE_LIST:
    next_line();
    md_list(node_current);
    break;
  case CMARK_NODE_ITEM:
    next_line();
    md_item(node_current);
    break;
  case CMARK_NODE_CODE_BLOCK:
    next_line();
    md_code_block();
    break;
  case CMARK_NODE_HTML_BLOCK:
    next_line();
    md_html_block(node_current);
    break;
  case CMARK_NODE_CUSTOM_BLOCK:
    next_line();
    md_custom_block(node_current);
    break;
  case CMARK_NODE_THEMATIC_BREAK:
    next_line();
    md_thematic_break(node_current);
    break;
  // -- Inline nodes --
  case CMARK_NODE_TEXT:
    md_text(node_current);
    break;
  case CMARK_NODE_SOFTBREAK:
    next_line();
    break;
  case CMARK_NODE_LINEBREAK:
    next_line();
    break;
  case CMARK_NODE_CODE:
    md_code();
    break;
  case CMARK_NODE_HTML_INLINE:
    md_html_inline(node_current);
    break;
  case CMARK_NODE_CUSTOM_INLINE:
    md_custom_inline(node_current);
    break;
  case CMARK_NODE_EMPH:
    md_emph();
    break;
  case CMARK_NODE_STRONG:
    md_strong(node_current);
    break;
  case CMARK_NODE_LINK:
    md_link();
    break;
  case CMARK_NODE_IMAGE:
    md_image(node_current);
    break;
  default:
    md_unknown(node_current);
    break;
  }

    // Рекурсивный обход
    cmark_node* child = cmark_node_first_child(node_current);
    while (child) {
        this->node_current = child;
        deploy_md_node();
        child = cmark_node_next(child);
    }
}

wxMenu* TxtCtl::edit_menu()
{
    wxWindow* topLevel = wxGetTopLevelParent(this);
    wxMenu* editMenu = new wxMenu;
    
    editMenu->Append(RICHTEXT_LEFT_ALIGN, _("Left Align"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ on_left_align(event); }, RICHTEXT_LEFT_ALIGN);
    
    editMenu->Append(RICHTEXT_RIGHT_ALIGN, _("Right Align"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ on_right_align(event); }, RICHTEXT_RIGHT_ALIGN);
    
    editMenu->Append(RICHTEXT_CENTRE, _("Centre"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnCentre(event); }, RICHTEXT_CENTRE);
    
    editMenu->Append(RICHTEXT_JUSTIFY, _("Justify"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnJustify(event); }, RICHTEXT_JUSTIFY);
    
    editMenu->AppendSeparator();
    
    editMenu->Append(RICHTEXT_CHANGE_FONT, _("Change Font"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ on_change_font(event); }, RICHTEXT_CHANGE_FONT);
    
    editMenu->Append(RICHTEXT_CHANGE_TEXT_COLOUR, _("Change Text Colour"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnChangeTextColour(event); }, RICHTEXT_CHANGE_TEXT_COLOUR);
    
    editMenu->Append(RICHTEXT_CHANGE_BACKGROUND_COLOUR, _("Change Background Colour"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnChangeBackgroundColour(event); }, RICHTEXT_CHANGE_BACKGROUND_COLOUR);
    
    editMenu->AppendSeparator();
    
    editMenu->Append(RICHTEXT_LEFT_INDENT, _("Left Indent"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnLeftIndent(event); }, RICHTEXT_LEFT_INDENT);

    editMenu->Append(RICHTEXT_RIGHT_INDENT, _("Right Indent"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnRightIndent(event); }, RICHTEXT_RIGHT_INDENT);

    editMenu->Append(RICHTEXT_TAB_STOPS, _("Tab Stops"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnTabStops(event); }, RICHTEXT_TAB_STOPS);

    return editMenu;
}


void TxtCtl::on_change_font(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;
    data.EnableEffects(true);
    wxFontDialog dialog(this, data);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        wxFont font = retData.GetChosenFont();
        wxColour colour = retData.GetColour();

        wxTextAttr attr;
        attr.SetFont(font);
        if (colour.IsOk())
            attr.SetTextColour(colour);

        long start, end;
        this->GetSelection(& start, & end);
        this->SetStyle(start, end, attr);
    }
}

void TxtCtl::on_left_align(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    long start, end;
    this->GetSelection(& start, & end);
    this->SetStyle(start, end, attr);
}

void TxtCtl::on_right_align(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);

    long start, end;
    this->GetSelection(& start, & end);
    this->SetStyle(start, end, attr);
}

void TxtCtl::OnJustify(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);

    long start, end;
    this->GetSelection(& start, & end);
    this->SetStyle(start, end, attr);
}

void TxtCtl::OnCentre(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);

    long start, end;
    this->GetSelection(& start, & end);
    this->SetStyle(start, end, attr);
}


void TxtCtl::OnChangeTextColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetColour(* wxBLACK);
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle("Choose the text colour");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();

        wxTextAttr attr;
        attr.SetTextColour(col);

        long start, end;
        this->GetSelection(& start, & end);
        this->SetStyle(start, end, attr);
    }
}

void TxtCtl::OnChangeBackgroundColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetColour(* wxWHITE);
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle("Choose the text background colour");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();

        wxTextAttr attr;
        attr.SetBackgroundColour(col);

        long start, end;
        this->GetSelection(& start, & end);
        this->SetStyle(start, end, attr);
    }
}

void TxtCtl::OnLeftIndent(wxCommandEvent& WXUNUSED(event))
{
    wxString indentStr = wxGetTextFromUser
                         (
                            _("Please enter the left indent in tenths of a millimetre."),
                            _("Left Indent"),
                            wxEmptyString,
                            this
                         );
    if (!indentStr.IsEmpty())
    {
        int indent = wxAtoi(indentStr);

        wxTextAttr attr;
        attr.SetLeftIndent(indent);

        long start, end;
        this->GetSelection(& start, & end);
        this->SetStyle(start, end, attr);
    }
}

void TxtCtl::OnRightIndent(wxCommandEvent& WXUNUSED(event))
{
    wxString indentStr = wxGetTextFromUser
                         (
                            _("Please enter the right indent in tenths of a millimetre."),
                            _("Right Indent"),
                            wxEmptyString,
                            this
                         );
    if (!indentStr.IsEmpty())
    {
        int indent = wxAtoi(indentStr);

        wxTextAttr attr;
        attr.SetRightIndent(indent);

        long start, end;
        this->GetSelection(& start, & end);
        this->SetStyle(start, end, attr);
    }
}

void TxtCtl::OnTabStops(wxCommandEvent& WXUNUSED(event))
{
    wxString tabsStr = wxGetTextFromUser
    (
        _("Please enter the tab stop positions in tenths of a millimetre, separated by spaces.\nLeave empty to reset tab stops."),
        _("Tab Stops"), wxEmptyString, this
    );

    wxArrayInt tabs;

    wxStringTokenizer tokens(tabsStr, " ");
    while (tokens.HasMoreTokens())
    {
        wxString token = tokens.GetNextToken();
        tabs.Add(wxAtoi(token));
    }

    wxTextAttr attr;
    attr.SetTabs(tabs);

    long start, end;
    this->GetSelection(& start, & end);
    this->SetStyle(start, end, attr);
}
