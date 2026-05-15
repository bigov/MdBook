#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/config.h"

#include "app_frame.h"

class MyApp: public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() ) return false;
    AppFrame* frame = new AppFrame("Hyper Markdown Book");
    frame->Show(true);

    return true;
}
