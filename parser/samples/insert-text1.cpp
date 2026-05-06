// Предполагается: #include <wx/richtext/richtextctrl.h>
// m_rich — валидный указатель на ранее созданный wxRichTextCtrl

void InsertHeaderAndParagraphs(wxRichTextCtrl* m_rich)
{
    if (!m_rich) return;

    // Очистка
    m_rich->Clear();

    // Сброс дефолтного стиля
    wxRichTextAttr defaultAttr;
    defaultAttr.SetFlags(0);
    defaultAttr.SetFontFaceName(wxEmptyString);
    defaultAttr.SetFontSize(0);
    defaultAttr.SetFontWeight(wxFONTWEIGHT_NORMAL);
    defaultAttr.SetFontStyle(wxFONTSTYLE_NORMAL);
    defaultAttr.SetTextColour(*wxBLACK);
    m_rich->SetDefaultStyle(defaultAttr);
    m_rich->GetBuffer().SetDefaultStyle(defaultAttr);

    // --- 3) Заголовок ---
    wxRichTextAttr headingAttr = defaultAttr;
    headingAttr.SetFontSize(16);
    headingAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    headingAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
    m_rich->BeginStyle(headingAttr);
    m_rich->WriteText("Заголовок\n");
    m_rich->EndStyle();

    // --- 4) Параграф с коротким текстом ---
    wxRichTextAttr paraAttr = defaultAttr;
    paraAttr.SetFontSize(11);
    paraAttr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    m_rich->BeginStyle(paraAttr);
    m_rich->WriteText("Параграф с коротким текстом\n");
    m_rich->EndStyle();

    // --- 5) Параграф с одним жирным словом ---
    wxString before = "Параграф с ";
    wxString boldWord = "жирным";
    wxString after  = " текстом\n";

    // Вставляем сначала обычный фрагмент до жирного слова
    m_rich->BeginStyle(paraAttr);
    m_rich->WriteText(before);
    m_rich->EndStyle();

    // Вставляем жирное слово
    wxRichTextAttr boldAttr = paraAttr;
    boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    m_rich->BeginStyle(boldAttr);
    m_rich->WriteText(boldWord);
    m_rich->EndStyle();

    // Вставляем оставшийся текст параграфа
    m_rich->BeginStyle(paraAttr);
    m_rich->WriteText(after);
    m_rich->EndStyle();

    // Установить курсор в начало текста
    m_rich->SetInsertionPoint(0);
    m_rich->ShowPosition(0);
}
