#pragma once
#include "wx/richtext/richtextctrl.h"

class wxMenu;
class wxCommandEvent;

enum
{
    RICHTEXT_LEFT_ALIGN = 1100,
    RICHTEXT_RIGHT_ALIGN,
    RICHTEXT_CENTRE,
    RICHTEXT_JUSTIFY,
    RICHTEXT_CHANGE_FONT,
    RICHTEXT_CHANGE_TEXT_COLOUR,
    RICHTEXT_CHANGE_BACKGROUND_COLOUR,
    RICHTEXT_LEFT_INDENT,
    RICHTEXT_RIGHT_INDENT,
    RICHTEXT_TAB_STOPS
};

class TxtCtl: public wxRichTextCtrl
{
public:
    TxtCtl(wxWindow* parent);
    wxMenu* EditMenu();
    void LoadXMLHandler();
    void LoadXmlFile(const wxString filePath);
    void LoadMdFile(const wxString filePath);
    void LoadPlainFile(const wxString filePath);
    void SavePlainFile(const wxString filePath);
    void SaveXmlFile(const wxString filePath);

private:
    wxRichTextAttr plainStyle;
    void ApplyXmlTemplate(wxString& palain_text);
    void PushXmlData(const wxString& xml_data);

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
};
