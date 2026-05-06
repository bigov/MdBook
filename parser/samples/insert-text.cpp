// Предполагается: #include <wx/richtext/richtextctrl.h> и m_rich — валидный указатель на wxRichTextCtrl

void ResetAndPopulate(wxRichTextCtrl* m_rich)
{
    if (!m_rich) return;

    // 1) Очистка текста (и истории undo/redo)
    m_rich->Clear();

    // 2) Сброс стиля — установим дефолтные атрибуты для буфера и контролла
    wxRichTextAttr defaultAttr;
    defaultAttr.SetFlags(0);
    defaultAttr.SetFontFaceName(wxEmptyString);
    defaultAttr.SetFontSize(0);
    defaultAttr.SetFontWeight(wxFONTWEIGHT_NORMAL);
    defaultAttr.SetFontStyle(wxFONTSTYLE_NORMAL);
    defaultAttr.SetTextColour(*wxBLACK);

    m_rich->SetDefaultStyle(defaultAttr);
    m_rich->GetBuffer().SetDefaultStyle(defaultAttr);

    // Удобная лямбда для вставки параграфа с заданным атрибутом и текстом
    auto writeParagraph = [&](const wxString& text, const wxRichTextAttr& attr, bool addNewline = true)
    {
        long start = m_rich->GetInsertionPoint();
        m_rich->BeginStyle(attr);
        m_rich->WriteText(text + (addNewline ? "\n" : ""));
        m_rich->EndStyle();
        return start;
    };

    // 3) Добавление заголовка
    wxRichTextAttr headingAttr = defaultAttr;
    headingAttr.SetFontSize(16);
    headingAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    headingAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
    writeParagraph("Заголовок", headingAttr);

    // 4) Параграф с коротким текстом
    wxRichTextAttr paraAttr = defaultAttr;
    paraAttr.SetFontSize(11);
    paraAttr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    writeParagraph("Параграф с коротким текстом", paraAttr);

    // 5) Параграф с одним словом жирным: "Параграф с жирным текстом"
    wxString before = "Параграф с ";
    wxString boldWord = "жирным";
    wxString after = " текстом";

    // Вставляем весь текст как обычный параграф
    long insertPos = m_rich->GetInsertionPoint();
    m_rich->BeginStyle(paraAttr);
    m_rich->WriteText(before + boldWord + after + "\n");
    m_rich->EndStyle();

    // Вычисляем позиции и применяем жирный стиль только к слову
    long boldStart = insertPos + before.Length();
    long boldEnd = boldStart + boldWord.Length();
    wxRichTextAttr boldAttr;
    boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    m_rich->SetStyle(boldStart, boldEnd, boldAttr);

    // Поместить каретку в конец и показать
    m_rich->ShowPosition(m_rich->GetInsertionPoint());
}
