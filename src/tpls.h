#include "wx/string.h"

inline const wxString tpl_Body = wxString::FromUTF8(R"(<?xml version="1.0" encoding="UTF-8"?>
<richtext version="1.0.0.0" xmlns="http://www.wxwidgets.org">
  <paragraphlayout textcolor="#222222" bgcolor="#FFFFFF" fontpointsize="12" fontfamily="70" fontstyle="90" fontweight="400" fontunderlined="0" fontface="Segoe UI" alignment="1">
    <paragraph>
      <text>%CONTENT%</text>
    </paragraph>
  </paragraphlayout>
</richtext>
)");

inline const wxString tpl_NewRow = wxString::FromUTF8(R"(</text>
    </paragraph>
    <paragraph>
      <text>)");

// Заголовок XML страницы
inline const wxString tpl_header = wxString::FromUTF8(R"(<?xml version="1.0" encoding="UTF-8"?>
<richtext version="1.0.0.0" xmlns="http://www.wxwidgets.org">
  <paragraphlayout textcolor="#222222" bgcolor="#FFFFFF" fontpointsize="12" fontfamily="70" fontstyle="90" fontweight="400" fontunderlined="0" fontface="Segoe UI" alignment="1">
%CONTENT%  </paragraphlayout>
</richtext>
)");

// Текст параграфа
inline const wxString tpl_p = wxString::FromUTF8(R"(    <paragraph>
      <text>%CONTENT%</text>
    </paragraph>
)");

// Фрагмент кода в строке: `xxx`
inline const wxString tpl_lncode = wxString::FromUTF8(R"(</text>
      <text textcolor="#008000" fontpointsize="12" fontfamily="75" fontstyle="90" fontweight="600" fontunderlined="0" fontface="Cascadia Mono SemiBold">%CONTENT%</text>
      <text>
)");

// Шаблоны заголовков 1 - 6 уровней
static const wxString tpl_h[6] = {
  wxString::FromUTF8(R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="18" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)"),
  wxString::FromUTF8(R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="17" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)"),
  wxString::FromUTF8(R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="16" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)"),
  wxString::FromUTF8(R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="15" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)"),
  wxString::FromUTF8(R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="14" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)"),
  wxString::FromUTF8(R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="13" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)"),
};
