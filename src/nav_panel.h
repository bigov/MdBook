#pragma once

#include "wx/treectrl.h"

class NavPanel : public wxTreeCtrl
{
public:
    explicit NavPanel(wxWindow* parent);

private:
    void BuildDefaultTree();
};
