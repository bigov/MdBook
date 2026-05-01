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
