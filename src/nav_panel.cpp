#include "nav_panel.h"
#include "txt_rich.h"
#include "wx/dir.h"
#include "wx/filename.h"


NavPanel::NavPanel(wxWindow* parent, TxtRich* txtRich)
    : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                 wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_SINGLE | wxBORDER_NONE),
      m_txtRich(txtRich)
{
    // Привязываем событие выбора элемента в дереве
    Bind(wxEVT_TREE_SEL_CHANGED, &NavPanel::on_selection, this);
}

void NavPanel::load_directory(const wxString& dir)
{
    if(dir.IsEmpty()) return;

    DeleteAllItems();
    this->current_dir = dir;

    wxDir directory(dir);
    if (!directory.IsOpened())
        return;
    
    wxTreeItemId root = AddRoot(wxFileName(dir).GetFullName());
    populate_tree(dir, root);
    Expand(root);
}

void NavPanel::populate_tree(const wxString& path, wxTreeItemId parent)
{
    wxDir directory(path);
    if (!directory.IsOpened())
        return;
    
    wxString filename;
    bool cont = directory.GetFirst(&filename);
    
    while (cont)
    {
        wxString fullPath = path + wxFileName::GetPathSeparator() + filename;
        
        if (wxDir::Exists(fullPath))
        {
            // Это директория - добавляем как ветку и рекурсивно заполняем
            wxTreeItemId folder = AppendItem(parent, filename);
            populate_tree(fullPath, folder);
        }
        else
        {
            // Это файл - добавляем как лист с полным путем в данных
            wxTreeItemId fileItem = AppendItem(parent, filename);
            SetItemData(fileItem, new FileItemData(fullPath));
        }
        
        cont = directory.GetNext(&filename);
    }
}

void NavPanel::on_selection(wxTreeEvent& event)
{
    if (!m_txtRich) return;
    
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk()) return;
    
    // Проверяем, есть ли у элемента данные (это файл)
    FileItemData* pData = static_cast<FileItemData*>(GetItemData(itemId));
    if (pData)
    {
        this->current_file = pData->GetFilePath();
        m_txtRich->load_file(this->current_file);
    }
}
