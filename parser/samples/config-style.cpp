wxRichTextAttr paraAttr;
paraAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
paraAttr.SetFontSize(14);
// применить стиль к последующей вставке
m_rich->BeginStyle(paraAttr);
m_rich->WriteText("Заголовок");
m_rich->Newline(); // newline унаследует параграфные атрибуты
m_rich->EndStyle();
