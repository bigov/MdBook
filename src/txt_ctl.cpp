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
#include "cmark.h"

#include "md_parser.h"
#include "txt_ctl.h"
#include "tpls.h"

namespace
{
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


TxtCtl::TxtCtl(wxWindow* parent)
    : wxRichTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE)
{
    this->Clear();
    plainStyle.SetFlags(wxTEXT_ATTR_FONT | wxTEXT_ATTR_TEXT_COLOUR
         | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_ALIGNMENT);
    plainStyle.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    plainStyle.SetFont(this->GetFont());
    plainStyle.SetTextColour(this->GetForegroundColour());
    plainStyle.SetBackgroundColour(this->GetBackgroundColour());
    this->SetDefaultStyle(plainStyle);
    this->SetBasicStyle(plainStyle);
    this->SetAndShowDefaultStyle(plainStyle);
    this->SetInsertionPoint(0);
}

void TxtCtl::LoadXMLHandler()
{
    if (!wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_XML))
    {
        wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    }
}

// --- Save the buffer content as plain text ---
void TxtCtl::SavePlainFile(const wxString filePath)
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

void TxtCtl::SaveXmlFile(const wxString filePath)
{
    wxRichTextBuffer& buffer = this->GetBuffer();
    LoadXMLHandler();
    if (!buffer.SaveFile(filePath, wxRICHTEXT_TYPE_XML))
    {
        wxLogError(_("Cannot save rich buffer file '%s'."), filePath.wc_str());
    }
    return;
}

// --- Load the prepared XML data into the control's buffer ---
void TxtCtl::PushXmlData(const wxString& content)
{
    wxStringInputStream xml_stream(content);
    wxRichTextBuffer& buffer = this->GetBuffer();
    LoadXMLHandler();
    if (!buffer.LoadFile(xml_stream, wxRICHTEXT_TYPE_XML))
    {
        wxLogWarning(_("Cannot load XML from string."));
        return;
    }
    this->Refresh();
}

// --- Wrapping text in an XML template for loading into the control's buffer ---
void TxtCtl::ApplyXmlTemplate(wxString& plain_text)
{
    plain_text.Replace("\n", tpl_NewRow);
    wxString xml_content = tpl_Body;
    xml_content.Replace("%CONTENT%", plain_text);
    PushXmlData(xml_content);
}
 
// --- Load the plain text content from a file ---
void TxtCtl::LoadPlainFile(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString plain_text;
    load_file_content(filePath, plain_text);
    ApplyXmlTemplate(plain_text);
}

// --- Load the XML content from a file ---
void TxtCtl::LoadXmlFile(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString xml_content;
    load_file_content(filePath, xml_content);
    PushXmlData(xml_content);
}

// --- Load from the Markdown file text on ACT ---
void TxtCtl::LoadMdFile(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString plain_text;
    load_file_content(filePath, plain_text);

    const wxScopedCharBuffer utf8 = plain_text.ToUTF8();
    const char *mdUtf8 = utf8.data() ? utf8.data() : "";
    cmark_node *buffer = cmark_parse_document(mdUtf8, strlen(mdUtf8), CMARK_OPT_DEFAULT);
    if (!buffer)
    { 
        wxLogWarning(_("Parse error in file '%s'."), filePath.wc_str());
        return; 
    }

    std::ostringstream oss;
    process_node(buffer, oss);
    cmark_node_free(buffer);

    wxString content = wxString::FromUTF8(oss.str().c_str());
    ApplyXmlTemplate(content);
}


wxMenu* TxtCtl::EditMenu()
{
    wxWindow* topLevel = wxGetTopLevelParent(this);
    wxMenu* editMenu = new wxMenu;
    
    editMenu->Append(RICHTEXT_LEFT_ALIGN, _("Left Align"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnLeftAlign(event); }, RICHTEXT_LEFT_ALIGN);
    
    editMenu->Append(RICHTEXT_RIGHT_ALIGN, _("Right Align"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnRightAlign(event); }, RICHTEXT_RIGHT_ALIGN);
    
    editMenu->Append(RICHTEXT_CENTRE, _("Centre"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnCentre(event); }, RICHTEXT_CENTRE);
    
    editMenu->Append(RICHTEXT_JUSTIFY, _("Justify"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnJustify(event); }, RICHTEXT_JUSTIFY);
    
    editMenu->AppendSeparator();
    
    editMenu->Append(RICHTEXT_CHANGE_FONT, _("Change Font"));
    if (topLevel)
        topLevel->Bind(wxEVT_MENU, [this](wxCommandEvent& event){ OnChangeFont(event); }, RICHTEXT_CHANGE_FONT);
    
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


void TxtCtl::OnChangeFont(wxCommandEvent& WXUNUSED(event))
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

void TxtCtl::OnLeftAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    long start, end;
    this->GetSelection(& start, & end);
    this->SetStyle(start, end, attr);
}

void TxtCtl::OnRightAlign(wxCommandEvent& WXUNUSED(event))
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
