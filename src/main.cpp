// Start of wxWidgets application code
#include <wx/wx.h>
#include "app_frame.h"
 
class MyApp : public wxApp
{
public:
    bool OnInit() override;
};
 
wxIMPLEMENT_APP(MyApp);
 
bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    AppFrame *frame = new AppFrame();
    frame->Show(true);
    return true;
}
