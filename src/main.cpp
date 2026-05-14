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

    int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

    wxConfig config("Book", "Hyper-Markdown");

    /* Методы класса wxConfig:  ReadLong, ReadBool, ReadDouble и общего Read для wxString.
      wxString name;
      config.Read("/User/Name", &name, "");
      double volume = config.ReadDouble("/User/Volume", 1.0);
      bool maximized = config.ReadBool("/MainWindow/Maximized", false);
     */

    // Чтение со значениями по умолчанию
    int win_width  = (int)config.ReadLong("/MainWindow/Width", 800L);
    int win_height = (int)config.ReadLong("/MainWindow/Height", 800L);
    int win_x = (int)config.ReadLong("/MainWindow/X", 10L);
    int win_y = (int)config.ReadLong("/MainWindow/Y", 10L);
    
    AppFrame* frame = new AppFrame("Hyper Markdown Book", win_x, win_y, win_width, win_height);
    frame->Show(true);

    return true;
}
