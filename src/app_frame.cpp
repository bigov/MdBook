#include "app_frame.h"

AppFrame::AppFrame(const wxString& title, int x, int y, int w, int h)
    : wxFrame(nullptr, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    wxInitAllImageHandlers();
    
    const wxString iconPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() 
        + ASSETS_DIR + wxFileName::GetPathSeparator() + APP_ICON_FNAME;
    SetAppIcon(iconPath);

    txt_ctl = new TxtCtl(this);
    m_currentPosition = -1;

    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(APP_CLOSE, _("Close\tCtrl+W"));
    Bind(wxEVT_MENU, &AppFrame::OnClose, this, APP_CLOSE);
    
    wxMenu* editMenu = txt_ctl->EditMenu();
    
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _("File"));
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
