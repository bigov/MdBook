#include "txt_ctl.h"

TxtCtl::TxtCtl(wxWindow* parent)
    : wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2)
{
    Init();
}

void TxtCtl::Init()
{
    wxString init_string;
    init_string << "\n title \n";
    init_string << "Some text with some style\n";
    SetValue(init_string);
}

void TxtCtl::OnChangeFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;
    wxFontDialog dialog(this, data);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        wxFont font = retData.GetChosenFont();

        wxTextAttr attr;
        attr.SetFont(font);

        long start, end;
        GetSelection(& start, & end);
        SetStyle(start, end, attr);

        //m_currentPosition = -1;
    }
}

void TxtCtl::OnLeftAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    long start, end;
    GetSelection(& start, & end);
    SetStyle(start, end, attr);

   //m_currentPosition = -1;
}

void TxtCtl::OnRightAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);

    long start, end;
    GetSelection(& start, & end);
    SetStyle(start, end, attr);

    //m_currentPosition = -1;
}

void TxtCtl::OnJustify(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);

    long start, end;
    GetSelection(& start, & end);
    SetStyle(start, end, attr);

    //m_currentPosition = -1;
}

void TxtCtl::OnCentre(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);

    long start, end;
    GetSelection(& start, & end);
    SetStyle(start, end, attr);

    //m_currentPosition = -1;
}


void TxtCtl::OnChangeTextColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetColour(* wxBLACK);
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle("Choose the text colour");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();

        wxTextAttr attr;
        attr.SetTextColour(col);

        long start, end;
        GetSelection(& start, & end);
        SetStyle(start, end, attr);

        //m_currentPosition = -1;
    }
}

void TxtCtl::OnChangeBackgroundColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetColour(* wxWHITE);
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle("Choose the text background colour");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();

        wxTextAttr attr;
        attr.SetBackgroundColour(col);

        long start, end;
        GetSelection(& start, & end);
        SetStyle(start, end, attr);

        //m_currentPosition = -1;
    }
}

void TxtCtl::OnLeftIndent(wxCommandEvent& WXUNUSED(event))
{
    wxString indentStr = wxGetTextFromUser
                         (
                            _("Please enter the left indent in tenths of a millimetre."),
                            _("Left Indent"),
                            wxEmptyString,
                            this
                         );
    if (!indentStr.IsEmpty())
    {
        int indent = wxAtoi(indentStr);

        wxTextAttr attr;
        attr.SetLeftIndent(indent);

        long start, end;
        GetSelection(& start, & end);
        SetStyle(start, end, attr);

        //m_currentPosition = -1;
    }
}

void TxtCtl::OnRightIndent(wxCommandEvent& WXUNUSED(event))
{
    wxString indentStr = wxGetTextFromUser
                         (
                            _("Please enter the right indent in tenths of a millimetre."),
                            _("Right Indent"),
                            wxEmptyString,
                            this
                         );
    if (!indentStr.IsEmpty())
    {
        int indent = wxAtoi(indentStr);

        wxTextAttr attr;
        attr.SetRightIndent(indent);

        long start, end;
        GetSelection(& start, & end);
        SetStyle(start, end, attr);

        //m_currentPosition = -1;
    }
}

void TxtCtl::OnTabStops(wxCommandEvent& WXUNUSED(event))
{
    wxString tabsStr = wxGetTextFromUser
                         (
                            _("Please enter the tab stop positions in tenths of a millimetre, separated by spaces.\nLeave empty to reset tab stops."),
                            _("Tab Stops"),
                            wxEmptyString,
                            this
                         );

    wxArrayInt tabs;

    wxStringTokenizer tokens(tabsStr, " ");
    while (tokens.HasMoreTokens())
    {
        wxString token = tokens.GetNextToken();
        tabs.Add(wxAtoi(token));
    }

    wxTextAttr attr;
    attr.SetTabs(tabs);

    long start, end;
    GetSelection(& start, & end);
    SetStyle(start, end, attr);

    //m_currentPosition = -1;
}

