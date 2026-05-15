#pragma once

#include "wx/panel.h"

class TxtRich;
class wxSimplebook;
class wxTextCtrl;

class MainPanel : public wxPanel
{
public:
    explicit MainPanel(wxWindow* parent);

    TxtRich* get_txt_rich() const;

private:
    wxSimplebook* m_book;
    TxtRich* m_txtRich;
    wxTextCtrl* m_plainText;
};
