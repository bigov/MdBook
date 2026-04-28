#pragma once

#include <wx/menu.h>

namespace app_menu_ids
{
inline constexpr int ID_Hello = wxID_HIGHEST + 1;
}

class AppMenu : public wxMenuBar
{
public:
	AppMenu();
};