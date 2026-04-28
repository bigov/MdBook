#include <wx/menu.h>

#include "app_menu.h"

AppMenu::AppMenu()
{
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuHelp = new wxMenu;

    menuFile->Append(app_menu_ids::ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    menuHelp->Append(wxID_ABOUT);

    Append(menuFile, "&File");
    Append(menuHelp, "&Help");
}
