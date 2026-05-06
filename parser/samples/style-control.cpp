#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/richtext/richtextxml.h>

class MyFrame : public wxFrame
{
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "wxRichTextCtrl: Formatting Example", wxDefaultPosition, wxSize(700, 500))
    {
        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        m_rich = new wxRichTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxVSCROLL | wxHSCROLL | wxTE_MULTILINE);
        sizer->Add(m_rich, 1, wxEXPAND | wxALL, 5);

        wxBoxSizer* btns = new wxBoxSizer(wxHORIZONTAL);
        btns->Add(new wxButton(panel, wxID_ANY, "Очистить"), 0, wxRIGHT, 5);
        btns->Add(new wxButton(panel, wxID_ANY, "Сброс стиля"), 0, wxRIGHT, 5);
        btns->Add(new wxButton(panel, wxID_ANY, "Добавить содержимое"), 0);
        sizer->Add(btns, 0, wxALIGN_LEFT | wxLEFT | wxBOTTOM, 5);

        panel->SetSizer(sizer);

        // биндинги (используем ID через GetChildren для простоты)
        wxWindowList& children = panel->GetChildren();
        wxWindowList::iterator it = children.begin();
        wxButton* bClear = static_cast<wxButton*>(*it); ++it;
        wxButton* bResetStyle = static_cast<wxButton*>(*it); ++it;
        wxButton* bAdd = static_cast<wxButton*>(*it);

        bClear->Bind(wxEVT_BUTTON, &MyFrame::OnClear, this);
        bResetStyle->Bind(wxEVT_BUTTON, &MyFrame::OnResetStyle, this);
        bAdd->Bind(wxEVT_BUTTON, &MyFrame::OnAddContent, this);

        // начальный пример текста (не обязателен)
        m_rich->WriteText("Исходный текст\n");
    }

private:
    void OnClear(wxCommandEvent& /*evt*/)
    {
        // 1) Очистка текста (удаляет содержимое и историю undo/redo)
        m_rich->Clear();
    }

    void OnResetStyle(wxCommandEvent& /*evt*/)
    {
        // 2) Сброс стиля: сброс текущих атрибутов к дефолтным.
        // Подход: установить текущие атрибуты в дефолтные через wxRichTextAttr
        wxRichTextAttr attr;
        attr.SetFlags(0); // нет специальных флагов
        attr.SetFontFaceName(wxEmptyString);
        attr.SetFontSize(0);    // ноль означает использование по умолчанию
        attr.SetFontWeight(wxFONTWEIGHT_NORMAL);
        attr.SetFontStyle(wxFONTSTYLE_NORMAL);
        attr.SetTextColour(*wxBLACK);
        // Устанавливаем как стиль для вставки/далее
        m_rich->SetStyle(m_rich->GetInsertionPoint(), m_rich->GetInsertionPoint(), attr);
        m_rich->SetDefaultStyle(attr);
        // также можно очистить выбор форматирования в буфере:
        wxRichTextBuffer& buf = m_rich->GetBuffer();
        buf.SetDefaultStyle(attr);
    }

    void OnAddContent(wxCommandEvent& /*evt*/)
    {
        // 3) Добавление строки заголовка.
        // Создадим атрибут для заголовка: крупный размер, жирный, центр.
        wxRichTextAttr headingAttr;
        headingAttr.SetFontSize(16);
        headingAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
        headingAttr.SetTextColour(*wxBLACK);
        headingAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
        // вставляем как параграф-заголовок
        long pos = m_rich->GetInsertionPoint();
        m_rich->BeginParagraphSpacing(0, 0);
        m_rich->BeginStyle(headingAttr);
        m_rich->WriteText("Заголовок\n");
        m_rich->EndStyle();
        m_rich->EndParagraphSpacing();

        // 4) Добавление параграфа с коротким текстом.
        wxRichTextAttr paraAttr;
        paraAttr.SetFontSize(11);
        paraAttr.SetFontWeight(wxFONTWEIGHT_NORMAL);
        paraAttr.SetTextColour(*wxBLACK);
        paraAttr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
        m_rich->BeginStyle(paraAttr);
        m_rich->WriteText("Параграф с коротким текстом\n");
        m_rich->EndStyle();

        // 5) Добавление параграфа с одним словом жирным:
        // Вставляем обычный текст, затем применяем стиль жирного к нужному слову.
        m_rich->BeginStyle(paraAttr);
        wxString before = "Параграф с ";
        wxString boldWord = "жирным";
        wxString after = " текстом\n";
        long start = m_rich->GetInsertionPoint();
        m_rich->WriteText(before + boldWord + after);
        long end = m_rich->GetInsertionPoint();

        // Найдём позицию вставленного жирного слова и применим к нему стиль
        long boldStart = start + before.Length();
        long boldEnd = boldStart + boldWord.Length();

        wxRichTextAttr boldAttr;
        boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
        // Применяем стиль к диапазону
        m_rich->SetStyle(boldStart, boldEnd, boldAttr);
        m_rich->EndStyle();

        // Переместим каретку после добавленного текста
        m_rich->ShowPosition(m_rich->GetInsertionPoint());
    }

    wxRichTextCtrl* m_rich;
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override
    {
        if (!wxApp::OnInit()) return false;
        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
