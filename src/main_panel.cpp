#include "main_panel.h"

#include "txt_rich.h"
#include "wx/button.h"
#include "wx/simplebook.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"

MainPanel::MainPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
{
    SetBackgroundColour(wxColour("#ffffff"));

    m_book = new wxSimplebook(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxBORDER_NONE
    );

    m_txtRich = new TxtRich(m_book);
    m_plainText = new wxTextCtrl(
        m_book,
        wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_MULTILINE | wxBORDER_NONE | wxTE_NO_VSCROLL
    );

    m_book->AddPage(m_txtRich, "txt_rich", true);
    m_book->AddPage(m_plainText, "source", false);

    wxPanel* tabsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxBORDER_NONE);
    tabsPanel->SetBackgroundColour(GetBackgroundColour());

    wxButton* tabBtnRich = new wxButton(tabsPanel, wxID_ANY, "txt_rich", wxDefaultPosition,
                                        wxDefaultSize, wxBU_EXACTFIT);
    wxButton* tabBtnText = new wxButton(tabsPanel, wxID_ANY, "source", wxDefaultPosition,
                                        wxDefaultSize, wxBU_EXACTFIT);

    tabBtnRich->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { m_book->SetSelection(0); });
    tabBtnText->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { m_book->SetSelection(1); });

    wxBoxSizer* tabsSizer = new wxBoxSizer(wxHORIZONTAL);
    tabsSizer->AddStretchSpacer(1);
    tabsSizer->Add(tabBtnRich, 0, wxRIGHT, 6);
    tabsSizer->Add(tabBtnText, 0);
    tabsPanel->SetSizer(tabsSizer);

    wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
    rootSizer->Add(m_book, 1, wxEXPAND | wxALL, 10);
    rootSizer->Add(tabsPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    SetSizer(rootSizer);
}

TxtRich* MainPanel::get_txt_rich() const
{
    return m_txtRich;
}
