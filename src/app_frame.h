#pragma once

#include <wx/wxprec.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/icon.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/colordlg.h>
#include <wx/fontdlg.h>
#include <wx/numdlg.h>
#include <wx/tokenzr.h>
#include <wx/frame.h>
#include <wx/string.h>



#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if wxUSE_CLIPBOARD
    #include <wx/dataobj.h>
    #include <wx/clipbrd.h>
#endif

#if wxUSE_FILE
    #include <wx/file.h>
#endif

#if wxUSE_TOOLTIPS
    #include <wx/tooltip.h>
#endif

// We test for wxUSE_DRAG_AND_DROP also, because data objects may not be
// implemented for compilers that can't cope with the OLE parts in
// wxUSE_DRAG_AND_DROP.
#if !wxUSE_DRAG_AND_DROP
    #undef wxUSE_CLIPBOARD
    #define wxUSE_CLIPBOARD 0
#endif


class wxCommandEvent;
class wxIdleEvent;
class wxTextCtrl;

class AppFrame: public wxFrame
{
public:
    AppFrame(const wxString& title, int x, int y, int w, int h);

    void SetAppIcon(const wxString& iconPath);
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