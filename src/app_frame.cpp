#include "app_frame.h"

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

wxBEGIN_EVENT_TABLE(AppFrame, wxFrame)
    EVT_IDLE(AppFrame::OnIdle)
    EVT_MENU(RICHTEXT_CLOSE, AppFrame::OnClose)
    EVT_MENU(RICHTEXT_LEFT_ALIGN, AppFrame::OnLeftAlign)
    EVT_MENU(RICHTEXT_RIGHT_ALIGN, AppFrame::OnRightAlign)
    EVT_MENU(RICHTEXT_CENTRE, AppFrame::OnCentre)
    EVT_MENU(RICHTEXT_JUSTIFY, AppFrame::OnJustify)
    EVT_MENU(RICHTEXT_CHANGE_FONT, AppFrame::OnChangeFont)
    EVT_MENU(RICHTEXT_CHANGE_TEXT_COLOUR, AppFrame::OnChangeTextColour)
    EVT_MENU(RICHTEXT_CHANGE_BACKGROUND_COLOUR, AppFrame::OnChangeBackgroundColour)
    EVT_MENU(RICHTEXT_LEFT_INDENT, AppFrame::OnLeftIndent)
    EVT_MENU(RICHTEXT_RIGHT_INDENT, AppFrame::OnRightIndent)
    EVT_MENU(RICHTEXT_TAB_STOPS, AppFrame::OnTabStops)
wxEND_EVENT_TABLE()

static const wxString ASSETS_DIR = "assets";
static const wxString APP_ICON_FNAME = "icon.png";

AppFrame::AppFrame(const wxString& title, int x, int y, int w, int h)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    wxInitAllImageHandlers();
    
    const wxString iconPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() 
        + ASSETS_DIR + wxFileName::GetPathSeparator() + APP_ICON_FNAME;
    SetAppIcon(iconPath);

    txt_ctl = new TxtCtl(this);
    m_currentPosition = -1;

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

void AppFrame::SetAppIcon(const wxString& iconPath)
{
    // Set the application icon
    if (wxFileExists(iconPath))
    {
        wxIcon appIcon;
        appIcon.CopyFromBitmap(wxBitmap(iconPath, wxBITMAP_TYPE_PNG));
        if (appIcon.IsOk())
        {
            SetIcon(appIcon);
        }
    }
}

void AppFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void AppFrame::OnLeftAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    long start, end;
    txt_ctl->GetSelection(& start, & end);
    txt_ctl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void AppFrame::OnRightAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);

    long start, end;
    txt_ctl->GetSelection(& start, & end);
    txt_ctl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void AppFrame::OnJustify(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);

    long start, end;
    txt_ctl->GetSelection(& start, & end);
    txt_ctl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void AppFrame::OnCentre(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);

    long start, end;
    txt_ctl->GetSelection(& start, & end);
    txt_ctl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void AppFrame::OnChangeFont(wxCommandEvent& WXUNUSED(event))
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
        txt_ctl->GetSelection(& start, & end);
        txt_ctl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void AppFrame::OnChangeTextColour(wxCommandEvent& WXUNUSED(event))
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
        txt_ctl->GetSelection(& start, & end);
        txt_ctl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void AppFrame::OnChangeBackgroundColour(wxCommandEvent& WXUNUSED(event))
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
        txt_ctl->GetSelection(& start, & end);
        txt_ctl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void AppFrame::OnLeftIndent(wxCommandEvent& WXUNUSED(event))
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
        txt_ctl->GetSelection(& start, & end);
        txt_ctl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void AppFrame::OnRightIndent(wxCommandEvent& WXUNUSED(event))
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
        txt_ctl->GetSelection(& start, & end);
        txt_ctl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void AppFrame::OnTabStops(wxCommandEvent& WXUNUSED(event))
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
    txt_ctl->GetSelection(& start, & end);
    txt_ctl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void AppFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    long insertionPoint = txt_ctl->GetInsertionPoint();
    if (insertionPoint != m_currentPosition)
    {
#if wxUSE_STATUSBAR
        wxTextAttr attr;
        if (txt_ctl->GetStyle(insertionPoint, attr))
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
