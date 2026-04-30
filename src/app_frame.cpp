#include "app_frame.h"
#include <wx/sstream.h>
#include <wx/richtext/richtextxml.h>
#include <wx/msgdlg.h>

namespace
{
static const wxString RICH_BUFFER_EXT = "wxrt";
static const wxString TEXT_BUFFER_EXT = "txt";
static const wxString FILE_DIALOG_WILDCARD = "Plain text files (*.txt)|*.txt|Rich text XML (*.wxrt)|*.wxrt";

wxString BuildRichSidecarPath(const wxString& filePath)
{
    wxFileName fileName(filePath);
    fileName.SetExt(RICH_BUFFER_EXT);
    return fileName.GetFullPath();
}

void EnsureXmlHandler()
{
    if (!wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_XML))
    {
        wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    }
}
}

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

void AppFrame::FileLoad(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog openFileDialog(this, _("Open File"), "", "",
                                "All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    auto filepath = openFileDialog.GetPath();

    // --- Load the file content as plain text ---
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
    // --- End load the file content as plain text ---

    // --- Load the file content as rich text ---
    const wxString richSidecarPath = BuildRichSidecarPath(filepath);
    if (wxFileExists(richSidecarPath))
    {
        wxRichTextBuffer& buffer = txt_ctl->GetBuffer();
        EnsureXmlHandler();
        if (buffer.LoadFile(richSidecarPath, wxRICHTEXT_TYPE_XML))
        {
            return;
        }

        wxLogWarning(_("Cannot load rich buffer sidecar '%s'. Falling back to plain text file."),
                     richSidecarPath);
    }
    else
    {
        wxMessageBox(
            wxString::Format(_("Buffer data file was not found:\n%s\n\nPlain text content will be loaded."), richSidecarPath.wc_str()),
            _("Information"),
            wxOK | wxICON_INFORMATION,
            this,
            wxDefaultCoord,
            wxDefaultCoord
        );
    }
}

void AppFrame::FileSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog
        saveFileDialog(this, _("Save file as"), "", "", FILE_DIALOG_WILDCARD,
                       wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
    
    const int fileType = saveFileDialog.GetFilterIndex();
    wxFileName fileName(saveFileDialog.GetPath());
    const wxString filePath = fileName.GetFullPath();
    if (fileName.GetExt().IsEmpty())
    {
        fileName.SetExt(fileType == 1 ? RICH_BUFFER_EXT : TEXT_BUFFER_EXT);
    }
    wxRichTextBuffer& buffer = txt_ctl->GetBuffer();

    // --- Save the buffer content as rich text ---
    if (fileType == 1)
    {
        EnsureXmlHandler();
        if (!buffer.SaveFile(filePath, wxRICHTEXT_TYPE_XML))
        {
            wxLogError(_("Cannot save rich buffer file '%s'."), filePath.wc_str());
        }
        return;
    }

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


void AppFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}
