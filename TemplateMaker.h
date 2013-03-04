
#ifndef TEMPLATEMAKER_H
#define TEMPLATEMAKER_H

#include "GradingTools.h"

GradingCategory CategoryDialog(wxWindow *parent);

class TemplateDeductionDialog: public wxDialog
{
  DECLARE_CLASS(TemplateDeductionDialog)

  protected:
  enum {
    ID_ADD_CRITERION = 6100,
    ID_CRITERION_TEXT = 6101,
    ID_SIMPLE = 6200,
    ID_COMPLEX = 6201
  };

  GradingDeduction *m_deduction;
  wxSizer *m_mainSizer;
  wxTextCtrl *m_nameCtrl;
  wxTextCtrl *m_mapCtrl;
  std::vector<wxTextCtrl *> m_critCtrls;
  bool m_simple;
  int m_currentCritID;

  void SetDeductionType(bool simple);
  void AddCriterion();

  void OnSetDeductionType(wxCommandEvent &e);
  void OnOK(wxCommandEvent &e);
  void OnCancel(wxCommandEvent &e);
  void OnAddCriterion(wxCommandEvent &e);

  public:
  TemplateDeductionDialog(wxWindow *parent, GradingDeduction *deduction);

  DECLARE_EVENT_TABLE()
};

class TemplateCategory: public wxPanel
{
  DECLARE_CLASS(TemplateCategory)

  protected:
  enum {
    ID_DEDUCTION = 6100,
    ID_ADD_DEDUCTION = 6200
  };

  GradingCategory m_category;
  wxSizer *m_mainSizer;
  int m_currentID;

  void AddDeduction();

  void OnAddDeduction(wxCommandEvent &e);

  public:
  TemplateCategory(wxWindow *parent, GradingCategory cat);

  GradingCategory &GetGradingCategory();

  DECLARE_EVENT_TABLE()
};

class TemplateMaker: public wxScrolledWindow
{
  DECLARE_CLASS(TemplatePanel)

  protected:
  enum {
    ID_CATEGORY = 6100,
    ID_ADD_CATEGORY = 6200
  };

  std::vector<TemplateCategory *> m_categories;
  wxSizer *m_mainSizer;

  void AddCategory();

  void OnAddCategory(wxCommandEvent &e);

  public:
  TemplateMaker(wxWindow *parent);

  void SaveTemplate();

  DECLARE_EVENT_TABLE()
};

#endif
