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
    void SetAppIcon(const wxString& iconPath);
};