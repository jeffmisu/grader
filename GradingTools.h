
#ifndef GRADINGTOOLS_H
#define GRADINGTOOLS_H

#include <wx/wx.h>
#include <vector>

#include <wx/bookctrl.h>
#include <wx/spinctrl.h>
#include <wx/richtext/richtextctrl.h>

class GradingDeduction
{
  public:
  GradingDeduction();
  GradingDeduction(std::string label);
  GradingDeduction(const GradingDeduction &d);

  GradingDeduction &operator=(const GradingDeduction &d);

  std::string m_label;
  float m_recorded;   // This deduction's current value in the total score sum.

  std::vector<wxCheckBox *> m_checkboxes;
  std::vector<std::string> m_choices; // For umbrella deductions.
  std::vector<float> m_mapping;       // The mapping from number of flaws to points taken off.

  void SetLabel(std::string label);
  void SetCheckbox(int index, bool state);
  void SetMapping(int index, float value);
  float GetValue();
  float GetMaxValue();

  void UpdateTotal(float *total);
  void AddChoice(std::string label);
  std::string Print();
  void Reset();

  wxSizer *BuildPanel(wxWindow *parent, int *currentID);

  std::string ToString();
};

class GradingCategory
{
  public:
  GradingCategory();
  GradingCategory(float value, std::string label, std::vector<GradingDeduction> dedux);
  GradingCategory(const GradingCategory &c);

  GradingCategory &operator=(const GradingCategory &c);

  std::vector<GradingDeduction> m_dedux;
  std::string m_label;
  float m_value;

  void UpdateTotal(float *total);
  void AddDeduction(GradingDeduction d);
  void SetDeductionBox(int deduction, int box, bool state);
  void Reset();

  std::string ToString();
};

class GradingPanel: public wxScrolledWindow
{
  DECLARE_CLASS(GradingPanel)

  protected:
  enum {
    ID_DEDUCTION = 6300,
    ID_NOTE = 6400
  };

  wxStaticText *m_pointsText;
  wxRichTextCtrl *m_notesText;

  float *m_total;
  std::vector<GradingDeduction *> m_deduxMapping;
  wxSizer *m_deduxBox;
  int m_currentID;

  void OnDeduction(wxCommandEvent &e);

  public:
  GradingPanel(wxWindow *parent, float *total);
  ~GradingPanel();

  void AddCategory(GradingCategory &cat);
  void SetPoints(float points);
  std::string GetNotes();
  void SetNotes(std::string notes);

  void Reset();

  friend class GradingTools;

  DECLARE_EVENT_TABLE()
};

class GradingText: public wxRichTextCtrl
{
  DECLARE_CLASS(GradingText)

  protected:
  std::string m_filename;

  public:
  GradingText(std::string filename, wxWindow *parent);
  ~GradingText();

  void Load(std::string filename);
  void Save();

  friend class GradingTools;

  DECLARE_EVENT_TABLE()
};

class GradingTools: public wxPanel
{
  DECLARE_CLASS(GradingTools)

  protected:
  GradingPanel *m_panel;
  std::vector<GradingText *> m_texts;
  wxNotebook *m_notebook;
  std::string m_filename;
  std::string m_templateFilename;
  int m_part;

  float m_totalPoints;
  float m_maxPoints;

  std::vector<GradingCategory> m_categories;

  public:
  GradingTools(int part, wxWindow *parent, std::string templateFilename);
  ~GradingTools();

  void LoadScoreSheet(std::string filename, bool build = false);
  void SaveScoreSheet();
  void SaveScoreFile();

  void OpenFiles(bool build = false);
  void BuildCategories(std::string content);
  void ParseScoreSheet(std::string content);

  void SetDeductionBox(int category, int deduction, int box, bool state);

  void UpdateDirectory();

  DECLARE_EVENT_TABLE()
};

#endif
