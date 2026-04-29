#include <wx/string.h>
#include <wx/frame.h>
#include <wx/textctrl.h>


class TxtCtl: public wxTextCtrl
{
public:
    TxtCtl(wxWindow* parent);

private:
	void Init();
};
