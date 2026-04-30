#pragma once
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/bitmap.h>

#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/filefn.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/stdpaths.h>

#include "txt_ctl.h"

static const int APP_CLOSE = 1000;
static const wxString ASSETS_DIR = "assets";
static const wxString APP_ICON_FNAME = "icon.png";

class AppFrame: public wxFrame
{
public:
    AppFrame(const wxString& title, int x, int y, int w, int h);

    void OnClose(wxCommandEvent& event);
    void FileLoad(wxCommandEvent& event);
    void FileSaveAs(wxCommandEvent& event);

private:
    TxtCtl* txt_ctl;
    long m_currentPosition;

    void SetAppIcon(const wxString& iconPath);
};