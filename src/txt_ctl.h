#include <wx/string.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/fontdlg.h>
#include <wx/numdlg.h>
#include <wx/event.h>
#include <wx/textdlg.h> 
#include <wx/tokenzr.h>


class TxtCtl: public wxTextCtrl
{
public:
    TxtCtl(wxWindow* parent);
    void OnChangeFont(wxCommandEvent& WXUNUSED(event));
    void OnLeftAlign(wxCommandEvent& event);
    void OnRightAlign(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);

private:
	void Init();
};
