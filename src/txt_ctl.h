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
    wxMenu* EditMenu();
    void LoadFile(const wxString filePath);
    void SavePlainFile(const wxString filePath);
    void SaveXmlFile(const wxString filePath);

private:
    wxRichTextAttr plainStyle;
    int row;
    
    void NewDocument();
    void PushXmlData(const wxString& xml_data);
    void MdNodeLoader(cmark_node* node);

    void next_row(cmark_node* n);
    void blank_row(int count);
    void show_literal(cmark_node* n);

    void MdNodeNone();
    void MdNodeDocument(cmark_node* n);
    void MdNodeBlockQuote(cmark_node* n);
    void MdNodeList(cmark_node* n);
    void MdNodeItem(cmark_node* n);
    void MdNodeCodeBlock(cmark_node* n);
    void MdNodeHtmlBlock(cmark_node* n);
    void MdNodeCustomBlock(cmark_node* n);
    void MdNodeParagraph(cmark_node* n);
    void MdNodeHeading(cmark_node* n);
    void MdNodeThematicBreak(cmark_node* n);
    void MdNodeText(cmark_node* n);
    void MdNodeSoftbreak(cmark_node* n);
    void MdNodeLinebreak(cmark_node* n);
    void MdNodeCode(cmark_node* n);
    void MdNodeHtmlInline(cmark_node* n);
    void MdNodeCustomInline(cmark_node* n);
    void MdNodeEmph(cmark_node* n);
    void MdNodeStrong(cmark_node* n);
    void MdNodeLink(cmark_node* n);
    void MdNodeImage(cmark_node* n);
    void MdNodeUnknown(cmark_node* n);

    void OnChangeFont(wxCommandEvent& WXUNUSED(event));
    void OnLeftAlign(wxCommandEvent& event);
    void OnRightAlign(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);
    void LoadXMLHandler();
    void LoadXmlFile(const wxString filePath);
    void LoadMdFile(const wxString filePath);
    void LoadPlainFile(const wxString filePath);
};
