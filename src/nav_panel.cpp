#include "nav_panel.h"

NavPanel::NavPanel(wxWindow* parent)
    : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                 wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_SINGLE | wxBORDER_NONE)
{
    BuildDefaultTree();
}

void NavPanel::BuildDefaultTree()
{
    const wxTreeItemId root = AddRoot("Document");
    AppendItem(root, "Heading 1");
    AppendItem(root, "Heading 2");
    AppendItem(root, "Heading 3");
    Expand(root);
}
