#pragma once
#include <wx/frame.h>
#include <wx/string.h>

#include "txt_ctl.h"

class AppFrame: public wxFrame
{
public:
    AppFrame(const wxString& title, int x, int y, int w, int h);

    void OnClose(wxCommandEvent& event);
    void FileLoad(wxCommandEvent& event);
    void FileSaveAs(wxCommandEvent& event);

private:
    TxtCtl* txt_ctl;
    long m_currentPosition;

    void SetAppIcon(const wxString& iconPath);
    void FileSaveTxt(wxRichTextBuffer& buffer, const wxString filePath);
    void FileSaveXml(wxRichTextBuffer& buffer, const wxString filePath);
    void FileLoadXml(const wxString filePath);
    void FileLoadMd(const wxString filePath);
};