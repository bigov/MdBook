/**
Related Classes (https://docs.wxwidgets.org/stable/overview_richtextctrl.html):

    Major classes: wxRichTextCtrl, wxRichTextBuffer, wxRichTextEvent
    Helper classes: wxTextAttr, wxRichTextRange
    File handler classes: wxRichTextFileHandler, wxRichTextHTMLHandler, wxRichTextXMLHandler
    Style classes: wxRichTextCharacterStyleDefinition, wxRichTextParagraphStyleDefinition, wxRichTextListStyleDefinition, wxRichTextStyleSheet
    Additional controls: wxRichTextStyleComboCtrl, wxRichTextStyleListBox, wxRichTextStyleListCtrl
    Printing classes: wxRichTextPrinting, wxRichTextPrintout, wxRichTextHeaderFooterData
    Dialog classes: wxRichTextStyleOrganiserDialog, wxRichTextFormattingDialog, wxSymbolPickerDialog
*/

#pragma once
#include <memory>
#include "wx/richtext/richtextctrl.h"

extern "C" {
#include "cmark.h"
}

class wxMenu;
class wxCommandEvent;

enum
{
    RICHTEXT_LEFT_ALIGN = 1100,
    RICHTEXT_RIGHT_ALIGN,
    RICHTEXT_CENTRE,
    RICHTEXT_JUSTIFY,
    RICHTEXT_CHANGE_FONT,
    RICHTEXT_CHANGE_TEXT_COLOUR,
    RICHTEXT_CHANGE_BACKGROUND_COLOUR,
    RICHTEXT_LEFT_INDENT,
    RICHTEXT_RIGHT_INDENT,
    RICHTEXT_TAB_STOPS
};

static const wxString RICH_BUFFER_EXT = "wxrt";
static const wxString TEXT_BUFFER_EXT = "txt";
static const wxString MARK_BUFFER_EXT = "md";

class TxtCtl: public wxRichTextCtrl
{
public:
    TxtCtl(wxWindow* parent);
    wxMenu* edit_menu();
    void load_file(const wxString filePath);
    void save_plain_file(const wxString filePath);
    void save_xml_file(const wxString filePath);

private:
    std::unique_ptr<wxRichTextStyleSheet> m_styleSheet;

    // wxTextAttrEx style_base;
    wxRichTextAttr style_base;
    wxRichTextAttr style_urls;
    wxRichTextAttr style_code;
    wxRichTextAttr style_code_block;

    int row_current;
    int row_total;
    cmark_node* node_current;
    
    void new_document();
    void push_xml_data(const wxString& xml_data);
    void deploy_md_node();

    void next_line();
    void row_check();
    void show_literal(cmark_node* n);
    void md_none();
    void md_blockquote(cmark_node* n);
    void md_list(cmark_node* n);
    void md_item(cmark_node* n);
    void md_code_block();
    void md_html_block(cmark_node* n);
    void md_custom_block(cmark_node* n);
    void md_header();
    void md_thematic_break(cmark_node* n);
    void md_text(cmark_node* n);
    void md_code();
    void md_html_inline(cmark_node* n);
    void md_custom_inline(cmark_node* n);
    void md_emph();
    void md_strong();
    void md_link();
    void md_image(cmark_node* n);
    void md_unknown(cmark_node* n);

    void on_change_font(wxCommandEvent& WXUNUSED(event));
    void on_left_align(wxCommandEvent& event);
    void on_right_align(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);
    void load_xml_handler();
    void load_xml_file(const wxString filePath);
    void load_md_file(const wxString filePath);
    void load_plain_file(const wxString filePath);
};
