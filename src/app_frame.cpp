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
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stdpaths.h"
#include "wx/config.h"

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

    // Панель для формирования полей (отступов) цвета фона
    // вокруг текстового контента, чтобы он не отображался
    // вплотную к границам фрейма.
    wxPanel* txt_border_panel = new wxPanel(this);
    txt_border_panel->SetBackgroundColour(wxColour("#ffffff"));

    txt_rich = new TxtRich(txt_border_panel);

    //Сюда верстается содержимое wxRichTextCtrl.
    wxBoxSizer* borderSizer = new wxBoxSizer(wxVERTICAL);
    borderSizer->Add(txt_rich, 1, wxEXPAND | wxALL, 10);
    txt_border_panel->SetSizer(borderSizer);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(txt_border_panel, 1, wxEXPAND | wxALL, 2);
    SetSizer(frameSizer);

    wxMenu* fileMenu = new wxMenu;

    fileMenu->Append(wxID_OPEN, _("Open File\tCtrl+O"));
    Bind(wxEVT_MENU, &AppFrame::FileLoad, this, wxID_OPEN);

    fileMenu->Append(wxID_SAVEAS, _("Save As...\tCtrl+S"));
    Bind(wxEVT_MENU, &AppFrame::FileSaveAs, this, wxID_SAVEAS);

    fileMenu->Append(APP_CLOSE, _("Exit\tCtrl+W"));
    Bind(wxEVT_MENU, &AppFrame::OnClose, this, APP_CLOSE);
    Bind(wxEVT_CLOSE_WINDOW, &AppFrame::OnWindowClose, this);
    
    //wxMenu* editMenu = txt_rich->edit_menu();
    
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _("File"));
    menuBar->Append(txt_rich->edit_menu(), _("Edit"));
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
    txt_rich->load_file(filepath);
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

    if (fileType == 0) txt_rich->save_plain_file(filePath);
    if (fileType == 1) txt_rich->save_xml_file(filePath);
}


void AppFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void AppFrame::OnWindowClose(wxCloseEvent& event)
{
    SaveWindowGeometry();
    event.Skip();
}

void AppFrame::SaveWindowGeometry()
{
    wxConfig config("Book", "Hyper-Markdown");

    const wxSize size = GetSize();
    const wxPoint pos = GetPosition();

    config.Write("/MainWindow/Width", (long)size.GetWidth());
    config.Write("/MainWindow/Height", (long)size.GetHeight());
    config.Write("/MainWindow/X", (long)pos.x);
    config.Write("/MainWindow/Y", (long)pos.y);
    config.Flush();
}
