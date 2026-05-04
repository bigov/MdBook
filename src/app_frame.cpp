#include "app_frame.h"
#include "wx/sstream.h"
#include "wx/filedlg.h"
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/log.h"
#include "wx/filefn.h"
#include "wx/icon.h"
#include "wx/image.h"
#include "wx/menu.h"
#include "wx/stdpaths.h"

static const int APP_CLOSE = 1000;
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

    wxMenu* fileMenu = new wxMenu;

    fileMenu->Append(wxID_OPEN, _("Open File\tCtrl+O"));
    Bind(wxEVT_MENU, &AppFrame::FileLoad, this, wxID_OPEN);

    fileMenu->Append(wxID_SAVEAS, _("Save As...\tCtrl+S"));
    Bind(wxEVT_MENU, &AppFrame::FileSaveAs, this, wxID_SAVEAS);

    fileMenu->Append(APP_CLOSE, _("Exit\tCtrl+W"));
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


void AppFrame::FileLoad(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog openFileDialog(this, _("Open File"), "", "",
                                "All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    auto filepath = openFileDialog.GetPath();
    txt_ctl->LoadFile(filepath);
}


void AppFrame::FileSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog
        saveFileDialog(this, _("Save file as"), "", "",
                      "Plain text files (*.txt)|*.txt|Rich text XML (*.wxrt)|*.wxrt",
                       wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
    
    const int fileType = saveFileDialog.GetFilterIndex();
    wxFileName fileName(saveFileDialog.GetPath());
    if (fileName.GetExt().IsEmpty())
    {
        fileName.SetExt(fileType == 1 ? RICH_BUFFER_EXT : TEXT_BUFFER_EXT);
    }
    
    const wxString filePath = fileName.GetFullPath();

    if (fileType == 0) txt_ctl->SavePlainFile(filePath);
    if (fileType == 1) txt_ctl->SaveXmlFile(filePath);
}


void AppFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}
