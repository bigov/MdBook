#pragma once

#include "wx/frame.h"
#include "wx/string.h"

class wxCommandEvent;
class wxIdleEvent;
class wxTextCtrl;

class AppFrame: public wxFrame
{
public:
    AppFrame(const wxString& title, int x, int y, int w, int h);

    void OnClose(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnLeftAlign(wxCommandEvent& event);
    void OnRightAlign(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeFont(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);

private:
    wxTextCtrl* m_textCtrl;
    long m_currentPosition;

    wxDECLARE_EVENT_TABLE();
};