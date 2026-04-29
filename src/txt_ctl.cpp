#include "txt_ctl.h"

TxtCtl::TxtCtl(wxWindow* parent)
    : wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2)
{
    Init();
}

void TxtCtl::Init()
{
    wxString init_string;
    init_string << "\nBOO.";
    SetValue(init_string);
}
