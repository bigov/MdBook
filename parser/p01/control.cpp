
void ApplyXmlTemplate(wxString& palain_text);

 
// --- Load from the Markdown file text ---
void TxtCtl::LoadMdFile(const wxString filePath)
{
    if (!isFileExist(filePath)) return;
    wxString plain_text;
    load_file_content(filePath, plain_text);

    const wxScopedCharBuffer utf8 = plain_text.ToUTF8();
    const char *mdUtf8 = utf8.data() ? utf8.data() : "";
    
    cmark_node *buffer = cmark_parse_document(mdUtf8, strlen(mdUtf8), CMARK_OPT_SOURCEPOS);
    if (!buffer)
    { 
        wxLogWarning(_("Parse error in file '%s'."), filePath.wc_str());
        return; 
    }

    std::ostringstream oss;
    process_node(buffer, oss);
    cmark_node_free(buffer);
    const std::string xml_utf8 = replace_placeholder(tpl_header, "%CONTENT%", oss.str());
    PushXmlData(wxString::FromUTF8(xml_utf8.c_str()));
}


// --- Wrapping text in an XML template for loading into the control's buffer ---
void TxtCtl::ApplyXmlTemplate(wxString& plain_text)
{
    std::string plain_utf8 = to_utf8(plain_text);
    plain_utf8 = replace_placeholder(plain_utf8, "\n", tpl_NewRow);
    const std::string xml_utf8 = replace_placeholder(tpl_Body, "%CONTENT%", plain_utf8);
    PushXmlData(wxString::FromUTF8(xml_utf8.c_str()));
}
