long start = m_rich->GetInsertionPoint();
m_rich->WriteText("Текст");
m_rich->Newline();
long end = m_rich->GetInsertionPoint();
m_rich->SetStyle(start, end, paraAttr); // параграфные атрибуты применятся к диапазону
