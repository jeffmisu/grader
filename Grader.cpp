
#include <wx/wx.h>
#include <wx/dirdlg.h>
#include <wx/aboutdlg.h>

#include "Grader.h"
#include "TemplateMaker.h"

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;
const int ID_NEXT = 501;
const int ID_PREV = 502;
const int ID_SAVE = 503;
const int ID_AUTOSAVE = 504;

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE;

// ----------------------------------------------------------------------------
// Event table for GraderFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GraderFrame, wxFrame)
  EVT_SIZE(GraderFrame::OnSize)
  EVT_MENU(wxID_ANY, GraderFrame::OnToolLeftClick)
  EVT_IDLE(GraderFrame::OnIdle)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// Main window
// ----------------------------------------------------------------------------

GraderFrame::GraderFrame(wxWindow *parent, const wxString &title, const wxPoint &pos, const wxSize &size, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
{
  m_tbar = NULL;

  SetIcon(wxICON(grdicon));

  m_autosave = false;

  m_panel = new wxPanel(this, wxID_ANY);
  m_panel->Show(true);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  m_panel->SetSizer(sizer);

  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-F4"), _T("Exit"));

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(wxID_ABOUT, _T("&About"), _T("About"));

  wxMenuBar *menuBar = new wxMenuBar(wxMB_DOCKABLE);
  menuBar->Append(fileMenu, _T("&File"));
  menuBar->Append(helpMenu, _T("&Help"));

  SetMenuBar(menuBar);

  RecreateToolbar();

  m_tools = NULL;
  m_maker = NULL;
}

void GraderFrame::OnExit(wxCommandEvent &WXUNUSED(event))
{
  Close(true);
}

GraderFrame *GraderFrame::Create(GraderFrame *parentFrame)
{
  wxString str = wxT("Let's grade some homework");

  wxSize size(800, 600);

  GraderFrame *frame = new GraderFrame(NULL, str, wxDefaultPosition, size);

  wxString choices[4] = {"Grade part one", "Grade part two", "Grade part three", "Create a score sheet"};
  wxSingleChoiceDialog partDlg(frame, "What are we doing here?", "Let me ask you something.", 4, choices, NULL, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxOK);

  if (partDlg.ShowModal() == wxID_CANCEL)
    exit(0);

  int part = partDlg.GetSelection() + 1;

  if (part == 4)
  {
    frame->m_maker = new TemplateMaker(frame->m_panel);
    frame->m_panel->GetSizer()->Add(frame->m_maker, 1, wxEXPAND, 0);
  }
  else if (part < 4 && part > 0)
  {
    wxFileDialog *fd = new wxFileDialog(NULL, "Find the grading template", "", "", "All files (*.*)|*.*");
    std::string templateFilename;
    while (fd->ShowModal() != wxID_OK);
    templateFilename = fd->GetPath();

    wxDirDialog(frame, "Now, show me the first student's directory, please!", "", wxDD_DEFAULT_STYLE | wxDD_CHANGE_DIR).ShowModal();

    std::string root = wxGetCwd().c_str();
    size_t pos = root.find_last_of("/\\");
    frame->m_currentStudent = &root[pos + 1];
    frame->SetLabel(frame->m_currentStudent);
    root.resize(pos);

    frame->m_root.Open(root);

    frame->m_tools = new GradingTools(part, frame->m_panel, templateFilename);
    frame->m_panel->GetSizer()->Add(frame->m_tools, 1, wxEXPAND, 0);
  }

  frame->m_panel->GetSizer()->Layout();

  frame->Show(true);
  frame->m_panel->Show(true);

  return frame;
}

void GraderFrame::LayoutChildren()
{
  wxSize csize = GetClientSize();

  if (m_tbar)
    m_tbar->SetSize(0, 0, wxDefaultCoord, wxDefaultCoord);

  if (m_panel)
    m_panel->SetSize(0, 0, csize.x, csize.y);
}

void GraderFrame::PopulateToolbar(wxToolBarBase *toolBar)
{
  enum
  {
    Tool_prev,
    Tool_next,
    Tool_save,
    Tool_auto,
    Tool_Max
  };

  wxBitmap toolBarBitmaps[Tool_Max];

  #define INIT_TOOL_BMP(bmp) toolBarBitmaps[Tool_##bmp] = wxBITMAP(bmp)
  INIT_TOOL_BMP(prev);
  INIT_TOOL_BMP(next);
  INIT_TOOL_BMP(save);
  INIT_TOOL_BMP(auto);

  int w = toolBarBitmaps[Tool_next].GetWidth(),
      h = toolBarBitmaps[Tool_next].GetHeight();

  toolBar->SetToolBitmapSize(wxSize(w, h));

  toolBar->AddTool(ID_PREV, _T("Previous"),
                   toolBarBitmaps[Tool_prev], wxNullBitmap, wxITEM_NORMAL,
                   _T("Previous student"), _T("Go to the previous student"));
  toolBar->AddTool(ID_NEXT, _T("Next"),
                   toolBarBitmaps[Tool_next], wxNullBitmap, wxITEM_NORMAL,
                   _T("Next student"), _T("Go to the next student"));
  toolBar->AddTool(ID_SAVE, _T("Save"),
                   toolBarBitmaps[Tool_save], wxNullBitmap, wxITEM_NORMAL,
                   _T("Save"), _T("Save the current student's score sheet"));
  toolBar->AddTool(ID_AUTOSAVE, _T("Autosave"),
                   toolBarBitmaps[Tool_auto], wxNullBitmap, wxITEM_CHECK,
                   _T("Autosave"), _T("When enabled, automatically saves the current student before switching to the next"));

  toolBar->Realize();
}

void GraderFrame::RecreateToolbar()
{
  wxToolBarBase *toolBar = GetToolBar();
  long style = toolBar ? toolBar->GetWindowStyle() : TOOLBAR_STYLE;

  delete toolBar;

  SetToolBar(NULL);
  toolBar = CreateToolBar(style, ID_TOOLBAR);
  PopulateToolbar(toolBar);
}

void GraderFrame::ShiftStudent(int d)
{
  if (d != 1 && d != -1)
    return;

  wxSize size = GetSize();

  wxDir root(m_root.GetName());
  wxString filename;
  wxString last = "";

  bool more = m_root.GetFirst(&filename, "", wxDIR_DIRS);
  while (more)
  {
    if (filename == m_currentStudent)
    {
      if (d == -1 && last.length() == 0)
        return;
      else if (d == -1)
      {
        m_currentStudent = last;
        break;
      }
      else if (d == 1)
      {
        more = m_root.GetNext(&filename);
        if (!more)
          return;
        m_currentStudent = filename;
        break;
      }
    }
    last = filename;
    more = m_root.GetNext(&filename);
  }

  if (m_autosave)
    m_tools->SaveScoreSheet();

  wxSetWorkingDirectory(m_root.GetName() + '/' + m_currentStudent);
  m_tools->UpdateDirectory();
  SetLabel(m_currentStudent);

  wxSize newSize = GetSize();

  newSize.Set(std::max(size.GetX(), newSize.GetX()), std::max(size.GetY(), newSize.GetY()));
  SetSize(newSize);
  LayoutChildren();
}

void GraderFrame::OnSize(wxSizeEvent &event)
{
  LayoutChildren();
}

void GraderFrame::OnIdle(wxIdleEvent &event)
{
  event.RequestMore();
}

void GraderFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
  Close(true);
}

void GraderFrame::OnToolLeftClick(wxCommandEvent &event)
{
  int id = event.GetId();

  if (id == ID_NEXT)
    ShiftStudent(1);
  else if (id == ID_PREV)
    ShiftStudent(-1);
  else if (id == ID_SAVE)
  {
    if (m_tools)
      m_tools->SaveScoreSheet();
    if (m_maker)
      m_maker->SaveTemplate();
  }
  else if (id == ID_AUTOSAVE)
    m_autosave = event.IsChecked();
  else if (id == wxID_EXIT)
    OnExit(event);
  else if (id == wxID_ABOUT)
    OnAbout(event);
}

void GraderFrame::OnAbout(wxCommandEvent &event)
{
  wxAboutDialogInfo info;
  info.SetName("CS111GA2K+");
  info.AddDeveloper("Jeff Sullivan");
  info.SetDescription("It's a program to make grading for CS111 at BU easier. Don't look too closely at the \
source code or you might have nightmares. If you find a horrible bug that ruins everything, or just one that \
annoys you, you can tell me about it at jeffs@bu.edu.");

  wxAboutBox(info);
}

// ----------------------------------------------------------------------------
// The application object
// ----------------------------------------------------------------------------

IMPLEMENT_APP(GraderApp)

bool GraderApp::OnInit()
{
  m_frame = GraderFrame::Create(NULL);

  return true;
}
