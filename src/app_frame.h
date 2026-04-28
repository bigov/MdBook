#pragma once

#include <wx/frame.h>

class wxCommandEvent;

class AppFrame : public wxFrame
{
public:
    AppFrame();

private:
    void SetAppIcon(void);
    void SetAppMenuBar(void);
    void SetupAppStatusBar(void);

    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};