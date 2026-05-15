#pragma once

#include "wx/treectrl.h"

class TxtRich;  // Forward declaration

// Класс для хранения пути файла в элементе дерева
class FileItemData : public wxTreeItemData
{
public:
    explicit FileItemData(const wxString& filePath) : m_filePath(filePath) {}
    const wxString& GetFilePath() const { return m_filePath; }

private:
    wxString m_filePath;
};

class NavPanel : public wxTreeCtrl
{
public:
    wxString current_dir;
    wxString current_file;
    explicit NavPanel(wxWindow* parent, TxtRich* txtRich = nullptr);
    void load_directory(const wxString& dir);

private:
    TxtRich* m_txtRich;
    void populate_tree(const wxString& path, wxTreeItemId parent);
    void on_selection(wxTreeEvent& event);
};
