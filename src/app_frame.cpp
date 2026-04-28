#include "app_frame.h"
#include "app_menu.h"

#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>

AppFrame::AppFrame()
    : wxFrame(nullptr, wxID_ANY, "Markdown Book")
{
    SetAppIcon();
    SetAppMenuBar();
    SetupAppStatusBar();
}

void AppFrame::SetAppIcon(void)
{
    // Set the application icon
    const wxString iconPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + "assets/" + "icon.png";
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

void AppFrame::SetAppMenuBar(void)
{
    // Set the application menu bar
    wxMenuBar* menuBar = new AppMenu();
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &AppFrame::OnHello, this, app_menu_ids::ID_Hello);
    Bind(wxEVT_MENU, &AppFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &AppFrame::OnExit, this, wxID_EXIT);
}

void AppFrame::SetupAppStatusBar(void)
{
    CreateStatusBar();
    SetStatusText("Welcome to Markdown Book!");
}

void AppFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello from Markdown Book!");
}

void AppFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void AppFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Markdown Book application\n",
                 "About Markdown Book", wxOK | wxICON_INFORMATION);
}

