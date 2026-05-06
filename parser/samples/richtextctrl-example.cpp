/*
 Пример со страницы документации: https://docs.wxwidgets.org/stable/overview_richtextctrl.html
 
*/

wxRichTextCtrl* richTextCtrl = new wxRichTextCtrl(
    splitter, wxID_ANY, wxEmptyString, wxDefaultPosition,
    wxSize(200, 200), wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);
 
wxFont textFont = wxFont(12, wxROMAN, wxNORMAL, wxNORMAL);
wxFont boldFont = wxFont(12, wxROMAN, wxNORMAL, wxBOLD);
wxFont italicFont = wxFont(12, wxROMAN, wxITALIC, wxNORMAL);
 
wxFont font(12, wxROMAN, wxNORMAL, wxNORMAL);
 
m_richTextCtrl->SetFont(font);
 
wxRichTextCtrl& r = richTextCtrl;
 
r.BeginSuppressUndo();
 
r.BeginParagraphSpacing(0, 20);
 
r.BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
r.BeginBold();
 
r.BeginFontSize(14);
r.WriteText(wxT("Welcome to wxRichTextCtrl, a wxWidgets control for editing and presenting styled text and images"));
r.EndFontSize();
r.Newline();
 
r.BeginItalic();
r.WriteText(wxT("by Julian Smart"));
r.EndItalic();
 
r.EndBold();
 
r.Newline();
r.WriteImage(wxBitmap(zebra_xpm));
 
r.EndAlignment();
 
r.Newline();
r.Newline();
 
r.WriteText(wxT("What can you do with this thing? "));
r.WriteImage(wxBitmap(smiley_xpm));
r.WriteText(wxT(" Well, you can change text "));
 
r.BeginTextColour(wxColour(255, 0, 0));
r.WriteText(wxT("colour, like this red bit."));
r.EndTextColour();
 
r.BeginTextColour(wxColour(0, 0, 255));
r.WriteText(wxT(" And this blue bit."));
r.EndTextColour();
 
r.WriteText(wxT(" Naturally you can make things "));
r.BeginBold();
r.WriteText(wxT("bold "));
r.EndBold();
r.BeginItalic();
r.WriteText(wxT("or italic "));
r.EndItalic();
r.BeginUnderline();
r.WriteText(wxT("or underlined."));
r.EndUnderline();
 
r.BeginFontSize(14);
r.WriteText(wxT(" Different font sizes on the same line is allowed, too."));
r.EndFontSize();
 
r.WriteText(wxT(" Next we'll show an indented paragraph."));
 
r.BeginLeftIndent(60);
r.Newline();
 
r.WriteText(wxT("Indented paragraph."));
r.EndLeftIndent();
 
r.Newline();
 
r.WriteText(wxT("Next, we'll show a first-line indent, achieved using BeginLeftIndent(100, -40)."));
 
r.BeginLeftIndent(100, -40);
r.Newline();
 
r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter."));
r.EndLeftIndent();
 
r.Newline();
 
r.WriteText(wxT("Numbered bullets are possible, again using subindents:"));
 
r.BeginNumberedBullet(1, 100, 60);
r.Newline();
 
r.WriteText(wxT("This is my first item. Note that wxRichTextCtrl doesn't automatically do numbering, but this will be added later."));
r.EndNumberedBullet();
 
r.BeginNumberedBullet(2, 100, 60);
r.Newline();
 
r.WriteText(wxT("This is my second item."));
r.EndNumberedBullet();
 
r.Newline();
 
r.WriteText(wxT("The following paragraph is right-indented:"));
 
r.BeginRightIndent(200);
r.Newline();
 
r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
r.EndRightIndent();
 
r.Newline();
 
wxArrayInt tabs;
tabs.Add(400);
tabs.Add(600);
tabs.Add(800);
tabs.Add(1000);
wxTextAttr attr;
attr.SetFlags(wxTEXT_ATTR_TABS);
attr.SetTabs(tabs);
r.SetDefaultStyle(attr);
 
r.WriteText(wxT("This line contains tabs:\tFirst tab\tSecond tab\tThird tab"));
 
r.Newline();
r.WriteText(wxT("Other notable features of wxRichTextCtrl include:"));
 
r.BeginSymbolBullet(wxT('*'), 100, 60);
r.Newline();
r.WriteText(wxT("Compatibility with wxTextCtrl API"));
r.EndSymbolBullet();
 
r.WriteText(wxT("Note: this sample content was generated programmatically from within the MyFrame constructor in the demo. The images were loaded from inline XPMs. Enjoy wxRichTextCtrl!"));
 
r.EndSuppressUndo();
