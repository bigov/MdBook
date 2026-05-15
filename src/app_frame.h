#pragma once
#include "wx/frame.h"
#include "wx/string.h"
#include "wx/splitter.h"

#include "txt_rich.h"

class NavPanel;

class AppFrame: public wxFrame
{
public:
    AppFrame(const wxString& title);

    void OnClose(wxCommandEvent& event);
    void OnWindowClose(wxCloseEvent& event);
    void OpenDir(wxCommandEvent& event);
    void FileSaveAs(wxCommandEvent& event);

private:
    TxtRich* txt_rich;
    NavPanel* nav_panel;
    wxSplitterWindow* splitter;
    void load_params();
    void save_params();
    void SetAppIcon(const wxString& iconPath);
};