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

    int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    int win_width = 800;
    int win_height = 800;
    int x = screenWidth/2 - win_width/2;
    int y = screenHeight/2 - win_height/2 - 120;

    AppFrame* frame = new AppFrame("Hyper Markdown Book", x, y, win_width, win_height);
    frame->Show(true);

    return true;
}
