// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_CLIPBOARD
    #include "wx/dataobj.h"
    #include "wx/clipbrd.h"
#endif

#if wxUSE_FILE
    #include "wx/file.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

// We test for wxUSE_DRAG_AND_DROP also, because data objects may not be
// implemented for compilers that can't cope with the OLE parts in
// wxUSE_DRAG_AND_DROP.
#if !wxUSE_DRAG_AND_DROP
    #undef wxUSE_CLIPBOARD
    #define wxUSE_CLIPBOARD 0
#endif

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/tokenzr.h"

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit() override;
};


class RichTextFrame: public wxFrame
{
public:
    RichTextFrame(const wxString& title, int x, int y, int w, int h);

// Event handlers

    void OnClose(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnLeftAlign(wxCommandEvent& event);
    void OnRightAlign(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeFont(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);

private:
    wxTextCtrl *m_textCtrl;
    long m_currentPosition;

    wxDECLARE_EVENT_TABLE();
};

//----------------------------------------------------------------------
// main()
//----------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);


enum
{
    RICHTEXT_CLOSE = 1000,
    RICHTEXT_LEFT_ALIGN,
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

wxBEGIN_EVENT_TABLE(RichTextFrame, wxFrame)
    EVT_IDLE(RichTextFrame::OnIdle)
    EVT_MENU(RICHTEXT_CLOSE, RichTextFrame::OnClose)
    EVT_MENU(RICHTEXT_LEFT_ALIGN, RichTextFrame::OnLeftAlign)
    EVT_MENU(RICHTEXT_RIGHT_ALIGN, RichTextFrame::OnRightAlign)
    EVT_MENU(RICHTEXT_CENTRE, RichTextFrame::OnCentre)
    EVT_MENU(RICHTEXT_JUSTIFY, RichTextFrame::OnJustify)
    EVT_MENU(RICHTEXT_CHANGE_FONT, RichTextFrame::OnChangeFont)
    EVT_MENU(RICHTEXT_CHANGE_TEXT_COLOUR, RichTextFrame::OnChangeTextColour)
    EVT_MENU(RICHTEXT_CHANGE_BACKGROUND_COLOUR, RichTextFrame::OnChangeBackgroundColour)
    EVT_MENU(RICHTEXT_LEFT_INDENT, RichTextFrame::OnLeftIndent)
    EVT_MENU(RICHTEXT_RIGHT_INDENT, RichTextFrame::OnRightIndent)
    EVT_MENU(RICHTEXT_TAB_STOPS, RichTextFrame::OnTabStops)
wxEND_EVENT_TABLE()

// MyFrame::MyFrame(const wxString& title, int x, int y)
//       : wxFrame(nullptr, wxID_ANY, title, wxPoint(x, y))
//       : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(300, 400))

RichTextFrame::RichTextFrame(const wxString& title, int x, int y, int w, int h)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    m_currentPosition = -1;
    m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
            wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2);

    wxString value;
    int i;
    for (i = 0; i < 10; i++)
    {
        int j;
        for (j = 0; j < 10; j++)
        {
            value << "Hello, welcome to a very simple rich text editor. You can set some character and paragraph styles from the Edit menu. ";
        }
        value << "\n\n";
    }
    m_textCtrl->SetValue(value);

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(RICHTEXT_CLOSE, _("Close\tCtrl+W"));
    menuBar->Append(fileMenu, _("File"));

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(RICHTEXT_LEFT_ALIGN, _("Left Align"));
    editMenu->Append(RICHTEXT_RIGHT_ALIGN, _("Right Align"));
    editMenu->Append(RICHTEXT_CENTRE, _("Centre"));
    editMenu->Append(RICHTEXT_JUSTIFY, _("Justify"));
    editMenu->AppendSeparator();
    editMenu->Append(RICHTEXT_CHANGE_FONT, _("Change Font"));
    editMenu->Append(RICHTEXT_CHANGE_TEXT_COLOUR, _("Change Text Colour"));
    editMenu->Append(RICHTEXT_CHANGE_BACKGROUND_COLOUR, _("Change Background Colour"));
    editMenu->AppendSeparator();
    editMenu->Append(RICHTEXT_LEFT_INDENT, _("Left Indent"));
    editMenu->Append(RICHTEXT_RIGHT_INDENT, _("Right Indent"));
    editMenu->Append(RICHTEXT_TAB_STOPS, _("Tab Stops"));
    menuBar->Append(editMenu, _("Edit"));

    SetMenuBar(menuBar);
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR
}

// Event handlers

void RichTextFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void RichTextFrame::OnLeftAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnRightAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnJustify(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnCentre(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnChangeFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;

    wxFontDialog dialog(this, data);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        wxFont font = retData.GetChosenFont();

        wxTextAttr attr;
        attr.SetFont(font);

        long start, end;
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnChangeTextColour(wxCommandEvent& WXUNUSED(event))
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
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnChangeBackgroundColour(wxCommandEvent& WXUNUSED(event))
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
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnLeftIndent(wxCommandEvent& WXUNUSED(event))
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
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnRightIndent(wxCommandEvent& WXUNUSED(event))
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
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnTabStops(wxCommandEvent& WXUNUSED(event))
{
    wxString tabsStr = wxGetTextFromUser
                         (
                            _("Please enter the tab stop positions in tenths of a millimetre, separated by spaces.\nLeave empty to reset tab stops."),
                            _("Tab Stops"),
                            wxEmptyString,
                            this
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
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    long insertionPoint = m_textCtrl->GetInsertionPoint();
    if (insertionPoint != m_currentPosition)
    {
#if wxUSE_STATUSBAR
        wxTextAttr attr;
        if (m_textCtrl->GetStyle(insertionPoint, attr))
        {
            wxString msg;
            wxString facename("unknown");
            if (attr.GetFont().IsOk())
            {
                facename = attr.GetFont().GetFaceName();
            }
            wxString alignment("unknown alignment");
            if (attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
                alignment = "centred";
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
                alignment = "right-aligned";
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
                alignment = "left-aligned";
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_JUSTIFIED)
                alignment = "justified";

            msg.Printf("Facename: %s", facename);

            if (attr.HasTextColour())
            {
                msg += wxString::Format(", colour: %s",
                                        attr.GetTextColour().GetAsString());
            }
            else
            {
                msg += ", no colour";
            }

            msg << ", " << alignment;

            if (attr.HasFont())
            {
                if (attr.GetFont().GetWeight() == wxFONTWEIGHT_BOLD)
                    msg += " BOLD";
                else if (attr.GetFont().GetWeight() == wxFONTWEIGHT_NORMAL)
                    msg += " NORMAL";

                if (attr.GetFont().GetStyle() == wxFONTSTYLE_ITALIC)
                    msg += " ITALIC";

                if (attr.GetFont().GetUnderlined())
                    msg += " UNDERLINED";
            }

            SetStatusText(msg);
        }
#endif // wxUSE_STATUSBAR
        m_currentPosition = insertionPoint;
    }
}


bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    RichTextFrame* frame = new RichTextFrame("Rich Text Editor", 50, 50, 600, 400);
    frame->Show(true);

    return true;
}
