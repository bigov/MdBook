#include "app_frame.h"
#include "wx/sstream.h"
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/filefn.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/stdpaths.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <cmark.h>

#include "md_parser.h"

static const int APP_CLOSE = 1000;
static const wxString ASSETS_DIR = "assets";
static const wxString APP_ICON_FNAME = "icon.png";
static const wxString RICH_BUFFER_EXT = "wxrt";
static const wxString TEXT_BUFFER_EXT = "txt";
static const wxString MARKDOWN_BUFFER_EXT = "md";
static const wxString FILE_DIALOG_WILDCARD = "Plain text files (*.txt)|*.txt|Rich text XML (*.wxrt)|*.wxrt";


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

void AppFrame::FileLoadMd(const wxString filePath)
{
    std::ifstream fin(filePath.ToStdString());
    if (!fin) { std::cerr << "Cannot open input file\n"; return; }
    std::string md((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

    cmark_node *doc = cmark_parse_document(md.c_str(), md.size(), CMARK_OPT_DEFAULT);
    if (!doc) { std::cerr << "Parse error\n"; return; }

    std::ostringstream oss;
    process_node(doc, oss);
    cmark_node_free(doc);

    txt_ctl->SetValue(wxString::FromUTF8(oss.str()));
}


// --- Load the file content as rich text ---
void AppFrame::FileLoadXml(const wxString filePath)
{
    if (wxFileExists(filePath))
    {
        txt_ctl->LoadXMLHandler();
        if (!txt_ctl->LoadFile(filePath, wxRICHTEXT_TYPE_XML))
        {
            wxLogWarning(_("Cannot load '%s'."), filePath.wc_str());
            return;
        }
        txt_ctl->Refresh();
    }
}

void AppFrame::FileLoad(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog openFileDialog(this, _("Open File"), "", "",
                                "All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    auto filepath = openFileDialog.GetPath();
    wxFileName fileName(filepath);
    wxString fileExt = fileName.GetExt();
    fileExt.LowerCase();
    if(fileExt == RICH_BUFFER_EXT)
    {
        FileLoadXml(filepath);
        return;
    }

    if(fileExt == MARKDOWN_BUFFER_EXT)
    {
        FileLoadMd(filepath);
        return;
    }

    wxFileInputStream input_stream(filepath);
    if (!input_stream.IsOk())
    {
        wxLogError(_("Cannot open file '%s'."), filepath);
        return;
    }

    wxString file_content;
    wxStringOutputStream string_stream(&file_content);
    input_stream.Read(string_stream);

    if (input_stream.GetLastError() != wxSTREAM_NO_ERROR &&
        input_stream.GetLastError() != wxSTREAM_EOF)
    {
        wxLogError(_("Cannot read file '%s'."), filepath);
        return;
    }

    txt_ctl->SetValue(file_content);
}

void AppFrame::FileSaveTxt(wxRichTextBuffer& buffer, const wxString filePath)
{
    // --- Save the buffer content as plain text ---
    const wxString plain_text = buffer.GetText().utf8_str();
    wxFileOutputStream output_stream(filePath);
    output_stream.Write(plain_text.data(), plain_text.length());

    if (output_stream.GetLastError() != wxSTREAM_NO_ERROR)
    {
        wxLogError(_("Error while writing to file '%s'."), filePath.wc_str());
        return;
    }
}

void AppFrame::FileSaveXml(wxRichTextBuffer& buffer, const wxString filePath)
{
    txt_ctl->LoadXMLHandler();
    if (!buffer.SaveFile(filePath, wxRICHTEXT_TYPE_XML))
    {
        wxLogError(_("Cannot save rich buffer file '%s'."), filePath.wc_str());
    }
    return;
}

void AppFrame::FileSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog
        saveFileDialog(this, _("Save file as"), "", "", FILE_DIALOG_WILDCARD,
                       wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
    
    const int fileType = saveFileDialog.GetFilterIndex();
    wxFileName fileName(saveFileDialog.GetPath());
    if (fileName.GetExt().IsEmpty())
    {
        fileName.SetExt(fileType == 1 ? RICH_BUFFER_EXT : TEXT_BUFFER_EXT);
    }
    
    const wxString filePath = fileName.GetFullPath();
    wxRichTextBuffer& buffer = txt_ctl->GetBuffer();

    if (fileType == 0) FileSaveTxt(buffer, filePath);
    if (fileType == 1) FileSaveXml(buffer, filePath);
}


void AppFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}
