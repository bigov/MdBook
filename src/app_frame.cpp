#include "app_frame.h"

enum
{
    APP_CLOSE = 1000,
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
    fileMenu->Append(APP_CLOSE, _("Close\tCtrl+W"));
    Bind(wxEVT_MENU, &AppFrame::OnClose, this, APP_CLOSE);
    menuBar->Append(fileMenu, _("File"));

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(RICHTEXT_LEFT_ALIGN, _("Left Align"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnLeftAlign(event);}, RICHTEXT_LEFT_ALIGN); 
    editMenu->Append(RICHTEXT_RIGHT_ALIGN, _("Right Align"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnRightAlign(event);}, RICHTEXT_RIGHT_ALIGN);
    editMenu->Append(RICHTEXT_CENTRE, _("Centre"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnCentre(event);}, RICHTEXT_CENTRE);
    editMenu->Append(RICHTEXT_JUSTIFY, _("Justify"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnJustify(event);}, RICHTEXT_JUSTIFY);
    editMenu->AppendSeparator();
    editMenu->Append(RICHTEXT_CHANGE_FONT, _("Change Font"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnChangeFont(event);}, RICHTEXT_CHANGE_FONT);
    editMenu->Append(RICHTEXT_CHANGE_TEXT_COLOUR, _("Change Text Colour"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnChangeTextColour(event);}, RICHTEXT_CHANGE_TEXT_COLOUR);
    editMenu->Append(RICHTEXT_CHANGE_BACKGROUND_COLOUR, _("Change Background Colour"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnChangeBackgroundColour(event);}, RICHTEXT_CHANGE_BACKGROUND_COLOUR);
    editMenu->AppendSeparator();
    editMenu->Append(RICHTEXT_LEFT_INDENT, _("Left Indent"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnLeftIndent(event);}, RICHTEXT_LEFT_INDENT);
    editMenu->Append(RICHTEXT_RIGHT_INDENT, _("Right Indent"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnRightIndent(event);}, RICHTEXT_RIGHT_INDENT);
    editMenu->Append(RICHTEXT_TAB_STOPS, _("Tab Stops"));
    Bind(wxEVT_MENU, [this](wxCommandEvent& event){txt_ctl->OnTabStops(event);}, RICHTEXT_TAB_STOPS);

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
