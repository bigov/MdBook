#include "app_frame.h"
#include "main_panel.h"
#include "nav_panel.h"
#include "wx/sstream.h"
#include "wx/filedlg.h"
#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/log.h"
#include "wx/filefn.h"
#include "wx/icon.h"
#include "wx/image.h"
#include "wx/menu.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stdpaths.h"
#include "wx/config.h"
#include "wx/dirdlg.h"
#include "wx/dir.h"

static const int APP_CLOSE = 1000;
static const wxString ASSETS_DIR = "assets";
static const wxString APP_ICON_FNAME = "icon.png";

AppFrame::AppFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
{
    wxInitAllImageHandlers();
    
    const wxString iconPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() 
        + ASSETS_DIR + wxFileName::GetPathSeparator() + APP_ICON_FNAME;
    SetAppIcon(iconPath);

    // Основной контейнер из двух областей: левая навигация и правый редактор.
    // Он отвечает за интерактивный разделитель (sash), который пользователь
    // перетаскивает мышью для изменения относительной ширины панелей.
    this->splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxSP_NO_XP_THEME);
    // Минимальная ширина каждой из двух областей, чтобы панели не схлопывались.
    this->splitter->SetMinimumPaneSize(120);
    // При изменении размера окна дополнительное место преимущественно получает
    // правая панель (редактор), левая навигация остается более стабильной.
    this->splitter->SetSashGravity(0.0);
    // Фон корневого окна синхронизируется с фоном splitter, чтобы боковые поля
    // по краям выглядели тем же цветом, что и область вокруг разделителя.
    SetBackgroundColour(this->splitter->GetBackgroundColour());

    MainPanel* main_panel = new MainPanel(this->splitter);
    txt_rich = main_panel->get_txt_rich();

    // Левая контейнерная панель навигации с такой же светлой theme-рамкой,
    // как у правой панели, чтобы визуально обе стороны выглядели одинаково.
    wxPanel* nav_border_panel = new wxPanel(this->splitter, wxID_ANY, wxDefaultPosition,
                                            wxDefaultSize, wxBORDER_THEME);
    // Дерево навигации, вложенное в левую контейнерную панель.
    nav_panel = new NavPanel(nav_border_panel, txt_rich);
    // Комфортная минимальная ширина панели навигации для читаемости заголовков.
    nav_panel->SetMinSize(wxSize(20, -1));

    // Компоновщик левой панели: дерево занимает всю полезную площадь контейнера.
    wxBoxSizer* navSizer = new wxBoxSizer(wxVERTICAL);
    navSizer->Add(nav_panel, 1, wxEXPAND);
    nav_border_panel->SetSizer(navSizer);

    // Размещение областей в splitter: слева навигация, справа редактор.
    // Третий аргумент задает начальную позицию разделителя по оси X.
    this->splitter->SplitVertically(nav_border_panel, main_panel);

    // Корневой компоновщик фрейма: размещает splitter на все окно, оставляя
    // только боковые внешние поля, без верхнего и нижнего отступа.
    wxBoxSizer* frameSizer = new wxBoxSizer(wxHORIZONTAL);
    frameSizer->Add(this->splitter, 1, wxEXPAND | wxLEFT | wxRIGHT, 3);
    SetSizer(frameSizer);

    wxMenu* fileMenu = new wxMenu;

    fileMenu->Append(wxID_OPEN, _("Open Dir\tCtrl+O"));
    Bind(wxEVT_MENU, &AppFrame::OpenDir, this, wxID_OPEN);

    fileMenu->Append(wxID_SAVEAS, _("Save As...\tCtrl+S"));
    Bind(wxEVT_MENU, &AppFrame::FileSaveAs, this, wxID_SAVEAS);

    fileMenu->Append(APP_CLOSE, _("Exit\tCtrl+W"));
    Bind(wxEVT_MENU, &AppFrame::OnClose, this, APP_CLOSE);
    Bind(wxEVT_CLOSE_WINDOW, &AppFrame::OnWindowClose, this);
    
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _("File"));
    menuBar->Append(txt_rich->edit_menu(), _("Edit"));
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    load_params();
}

void AppFrame::SetAppIcon(const wxString& iconPath)
{
    // Set the application icon
    if (wxFileExists(iconPath))
    {
        wxIcon appIcon;
        appIcon.CopyFromBitmap(wxBitmap(iconPath, wxBITMAP_TYPE_PNG));
        if (appIcon.IsOk())
        {
            SetIcon(appIcon);
        }
    }
}


void AppFrame::OpenDir(wxCommandEvent& WXUNUSED(event))
{
    wxString defaultDir = wxGetHomeDir(); // начальная папка
    wxDirDialog dlg(this, "Select directory", defaultDir, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK) {
        wxString path = dlg.GetPath();
        nav_panel->load_directory(path);
    }
}


void AppFrame::FileSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog
        saveFileDialog(this, _("Save file as"), "", "",
                      "Plain text files (*.txt)|*.txt|Rich text XML (*.wxrt)|*.wxrt",
                       wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
    
    const int fileType = saveFileDialog.GetFilterIndex();
    wxFileName fileName(saveFileDialog.GetPath());
    if (fileName.GetExt().IsEmpty())
    {
        fileName.SetExt(fileType == 1 ? RICH_BUFFER_EXT : TEXT_BUFFER_EXT);
    }
    
    const wxString filePath = fileName.GetFullPath();

    if (fileType == 0) txt_rich->save_plain_file(filePath);
    if (fileType == 1) txt_rich->save_xml_file(filePath);
}


void AppFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void AppFrame::OnWindowClose(wxCloseEvent& event)
{
    save_params();
    event.Skip();
}

void AppFrame::load_params()
{
    wxConfig config("Book", "Hyper-Markdown");
    /* Методы класса wxConfig:  ReadLong, ReadBool, ReadDouble и общего Read для wxString.
      wxString name;
      config.Read("/User/Name", &name, "");
      double volume = config.ReadDouble("/User/Volume", 1.0);
      bool maximized = config.ReadBool("/MainWindow/Maximized", false);
     */

    int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

    long width = (long)(screenWidth * 0.8); // 80% от ширины экрана
    long height = (long)(screenHeight * 0.8); // 80% от высоты экрана
    long x = 10, y = 10, d = 250;

    // Чтение со значениями по умолчанию
    int win_width  = (int)config.ReadLong("/MainWindow/Width", width);
    int win_height = (int)config.ReadLong("/MainWindow/Height", height);
    int win_x = (int)config.ReadLong("/MainWindow/X", x);
    int win_y = (int)config.ReadLong("/MainWindow/Y", y);
    int span = (int)config.ReadLong("/MainWindow/span", d);

    this->SetSize(win_x, win_y, win_width, win_height);
    this->splitter->SetSashPosition(span); // позиция разделителя
    this->nav_panel->load_directory(config.Read("/MainWindow/current_dir", wxEmptyString));
    this->txt_rich->load_file(config.Read("/MainWindow/current_file", wxEmptyString));
}

void AppFrame::save_params()
{
    wxConfig config("Book", "Hyper-Markdown");

    const wxSize size = GetSize();
    const wxPoint pos = GetPosition();

    config.Write("/MainWindow/Width", (long)size.GetWidth());
    config.Write("/MainWindow/Height", (long)size.GetHeight());
    config.Write("/MainWindow/X", (long)pos.x);
    config.Write("/MainWindow/Y", (long)pos.y);
    config.Write("/MainWindow/span", (long)this->splitter->GetSashPosition());
    config.Write("/MainWindow/current_dir", this->nav_panel->current_dir);
    config.Write("/MainWindow/current_file", this->nav_panel->current_file);
    config.Flush();
}
