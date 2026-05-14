#pragma once
#include "wx/frame.h"
#include "wx/string.h"

#include "txt_rich.h"

class NavPanel;

class AppFrame: public wxFrame
{
public:
    AppFrame(const wxString& title, int x, int y, int w, int h);

    void OnClose(wxCommandEvent& event);
    void OnWindowClose(wxCloseEvent& event);
    void FileLoad(wxCommandEvent& event);
    void FileSaveAs(wxCommandEvent& event);

private:
    TxtRich* txt_rich;
    NavPanel* nav_panel;
    void SaveWindowGeometry();
    void SetAppIcon(const wxString& iconPath);
};