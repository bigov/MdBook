#include <string>

inline const std::string tpl_Body = R"(<?xml version="1.0" encoding="UTF-8"?>
<richtext version="1.0.0.0" xmlns="http://www.wxwidgets.org">
  <paragraphlayout textcolor="#222222" bgcolor="#FFFFFF" fontpointsize="12" fontfamily="70" fontstyle="90" fontweight="400" fontunderlined="0" fontface="Segoe UI" alignment="1">
    <paragraph>
      <text>%CONTENT%</text>
    </paragraph>
  </paragraphlayout>
</richtext>
)";

inline const std::string tpl_NewRow = R"(</text>
    </paragraph>
    <paragraph>
      <text>)";

// Заголовок XML страницы
inline const std::string tpl_header = R"(<?xml version="1.0" encoding="UTF-8"?>
<richtext version="1.0.0.0" xmlns="http://www.wxwidgets.org">
  <paragraphlayout textcolor="#444444" bgcolor="#FFFFFF" fontpointsize="12" fontfamily="70" fontstyle="90" fontweight="400" fontunderlined="0" fontface="Segoe UI" alignment="1">
%CONTENT%  </paragraphlayout>
</richtext>
)";

// Текст параграфа
inline const std::string tpl_p = R"(    <paragraph>
      <text>%CONTENT%</text>
    </paragraph>
)";

// Strong (жирный)
inline const std::string tpl_s = R"(</text>
      <text textcolor="#000000" fontpointsize="12" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
      <text>)";

// Italic (курсив)
inline const std::string tpl_i = R"(</text>
      <text textcolor="#000000" fontpointsize="12" fontfamily="70" fontstyle="93" fontweight="500" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
      <text>)";

// Bold + Italic (вложенный ***text***)
inline const std::string tpl_bi = R"(</text>
      <text textcolor="#000000" fontpointsize="12" fontfamily="70" fontstyle="93" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
      <text>)";

// Фрагмент кода в строке: `xxx`
inline const std::string tpl_lncode = R"(</text>
      <text textcolor="#006000" bgcolor="#EFEFEF" fontpointsize="12" fontfamily="75" fontstyle="90" fontweight="200" fontunderlined="0" fontface="Cascadia Mono SemiBold">`%CONTENT%`</text>
      <text>
)";

// Шаблоны заголовков 1 - 6 уровней
static const std::string tpl_h[6] = {
  R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="18" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)" ,
  R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="17" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)" ,
  R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="16" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)" ,
  R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="15" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)" ,
  R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="14" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)" ,
  R"(    <paragraph>
      <text textcolor="#882222" fontpointsize="13" fontfamily="70" fontstyle="90" fontweight="700" fontunderlined="0" fontface="Segoe UI">%CONTENT%</text>
    </paragraph>
)" ,
};
