// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app_frame.h"

class MyApp: public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    AppFrame* frame = new AppFrame("Rich Text Editor", 50, 50, 600, 400);
    frame->Show(true);

    return true;
}
