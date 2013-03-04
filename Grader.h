
#ifndef GRADER_H
#define GRADER_H

#include <wx/toolbar.h>
#include <wx/dir.h>

#include "GradingTools.h"
#include "TemplateMaker.h"

class GraderFrame: public wxFrame
{
  public:
  static GraderFrame *Create(GraderFrame *parentFrame);

  void OnExit(wxCommandEvent& event);

  protected:
  GraderFrame(wxWindow *parent, const wxString &title, const wxPoint &pos,
    const wxSize &size, long style = wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE);

  public:
  void PopulateToolbar(wxToolBarBase* toolBar);
  void RecreateToolbar();

  void OnIdle(wxIdleEvent &event);
  void OnQuit(wxCommandEvent &event);
  void OnToolLeftClick(wxCommandEvent &event);
  void OnSize(wxSizeEvent &event);
  void OnAbout(wxCommandEvent &event);

  protected:
  void LayoutChildren();

  void ShiftStudent(int d);

  wxPanel *m_panel;
  wxToolBar *m_tbar;
  GradingTools *m_tools;
  TemplateMaker *m_maker;

  wxDir m_root;
  std::string m_currentStudent;
  bool m_autosave;

  DECLARE_EVENT_TABLE()
};

class GraderApp: public wxApp
{
  protected:
  GraderFrame *m_frame;

  public:
  bool OnInit();
};

#endif

