
#include "TemplateMaker.h"

#include <sstream>
#include <fstream>

GradingCategory CategoryDialog(wxWindow *parent)
{
  wxDialog dlg(parent, wxID_ANY, "Add category", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
  wxSizer *szr = new wxBoxSizer(wxVERTICAL);

  wxSizer *sub = new wxBoxSizer(wxHORIZONTAL);
  wxTextCtrl *nameCtrl = new wxTextCtrl(&dlg, wxID_ANY);
  sub->Add(new wxStaticText(&dlg, wxID_ANY, "Description:"), 1, wxALL | wxALIGN_CENTER, 4);
  sub->Add(nameCtrl, 0, wxALL | wxALIGN_CENTER, 4);
  sub->Layout();
  sub->SetSizeHints(&dlg);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  sub = new wxBoxSizer(wxHORIZONTAL);
  wxTextCtrl *valueCtrl = new wxTextCtrl(&dlg, wxID_ANY, "10.0");
  sub->Add(new wxStaticText(&dlg, wxID_ANY, "Point value:"), 1, wxALL | wxALIGN_CENTER, 4);
  sub->Add(valueCtrl, 0, wxALL | wxALIGN_CENTER, 4);
  sub->Layout();
  sub->SetSizeHints(&dlg);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  sub = new wxBoxSizer(wxHORIZONTAL);
  sub->Add(new wxButton(&dlg, wxID_OK, "OK"), 1, wxALIGN_BOTTOM | wxALIGN_RIGHT | wxALL, 8);
  sub->Add(new wxButton(&dlg, wxID_CANCEL, "Cancel"), 1, wxALIGN_BOTTOM | wxALIGN_RIGHT | wxALL, 8);
  sub->Layout();
  sub->SetSizeHints(&dlg);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  dlg.SetSizer(szr);
  szr->Layout();
  szr->SetSizeHints(&dlg);

  dlg.SetSize(dlg.GetSize() + wxSize(10, 10));
  dlg.ShowModal();

  float value;
  sscanf(valueCtrl->GetValue().c_str(), "%f", &value);

  return GradingCategory(value, nameCtrl->GetValue().c_str(), std::vector<GradingDeduction>());
}

//-----

IMPLEMENT_CLASS(TemplateDeductionDialog, wxDialog)

BEGIN_EVENT_TABLE(TemplateDeductionDialog, wxDialog)
  EVT_BUTTON(wxID_OK, TemplateDeductionDialog::OnOK)
  EVT_BUTTON(wxID_CANCEL, TemplateDeductionDialog::OnCancel)
  EVT_RADIOBUTTON(ID_SIMPLE, TemplateDeductionDialog::OnSetDeductionType)
  EVT_RADIOBUTTON(ID_COMPLEX, TemplateDeductionDialog::OnSetDeductionType)
  EVT_BUTTON(ID_ADD_CRITERION, TemplateDeductionDialog::OnAddCriterion)
END_EVENT_TABLE()

TemplateDeductionDialog::TemplateDeductionDialog(wxWindow *parent, GradingDeduction *deduction):
  wxDialog(parent, wxID_ANY, "Set up deduction", wxDefaultPosition, wxDefaultSize)
{
  m_deduction = deduction;
  m_mainSizer = NULL;
  m_currentCritID = ID_CRITERION_TEXT;
  m_simple = true;

  wxSizer *szr = new wxBoxSizer(wxVERTICAL);

  wxSizer *sub = new wxBoxSizer(wxHORIZONTAL);
  m_nameCtrl = new wxTextCtrl(this, wxID_ANY);
  sub->Add(new wxStaticText(this, wxID_ANY, "Description:"), 1, wxALL | wxALIGN_CENTER, 4);
  sub->Add(m_nameCtrl, 0, wxALL | wxALIGN_CENTER, 4);
  sub->Layout();
  sub->SetSizeHints(this);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  sub = new wxBoxSizer(wxHORIZONTAL);
  m_mapCtrl = new wxTextCtrl(this, wxID_ANY, "-1.0");
  sub->Add(new wxStaticText(this, wxID_ANY, "Point mapping:"), 1, wxALL | wxALIGN_CENTER, 4);
  sub->Add(m_mapCtrl, 0, wxALL | wxALIGN_CENTER, 4);
  sub->Layout();
  sub->SetSizeHints(this);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  sub = new wxBoxSizer(wxHORIZONTAL);
  sub->Add(new wxRadioButton(this, ID_SIMPLE, "Simple", wxDefaultPosition, wxDefaultSize, wxRB_GROUP), 1, wxALIGN_CENTER, 4);
  sub->Add(new wxRadioButton(this, ID_COMPLEX, "Multiple"), 1, wxALIGN_CENTER, 4);
  sub->Layout();
  sub->SetSizeHints(this);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  sub = new wxBoxSizer(wxHORIZONTAL);
  sub->Add(new wxButton(this, wxID_OK, "OK"), 1, wxALIGN_BOTTOM | wxALIGN_RIGHT | wxALL, 8);
  sub->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 1, wxALIGN_BOTTOM | wxALIGN_RIGHT | wxALL, 8);
  sub->Layout();
  sub->SetSizeHints(this);
  szr->Add(sub, 1, wxGROW | wxALIGN_CENTER);

  SetSizer(szr);
  szr->Layout();
  szr->SetSizeHints(this);
}

void TemplateDeductionDialog::SetDeductionType(bool simple)
{
  wxSizer *old = m_mainSizer;

  if (simple)
  {
    //m_mainSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Value");
    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainSizer->Add(new wxStaticText(this, wxID_ANY, "Value:"), 1, wxALL | wxALIGN_CENTER, 4);
    m_mainSizer->Add(new wxTextCtrl(this, wxID_ANY, "0"), 1, wxGROW | wxALL, 4);
    m_mainSizer->Layout();
  }
  else
  {
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    //m_mainSizer->Add(new wxStaticText(this, wxID_ANY, "You can add criteria later."), 1, wxALL | wxALIGN_CENTER, 8);
    m_mainSizer->Add(new wxButton(this, ID_ADD_CRITERION, "Add criterion"), 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
    m_mainSizer->Layout();
  }

  if (old)
  {
    old->Clear(true);
    GetSizer()->Remove(old);
  }

  GetSizer()->Insert(3, m_mainSizer, 0, wxALIGN_CENTER | wxGROW | wxALL, 4);

  GetSizer()->Layout();
  GetSizer()->SetSizeHints(this);

  m_simple = simple;
}

void TemplateDeductionDialog::AddCriterion()
{
  wxTextCtrl *critCtrl = new wxTextCtrl(this, m_currentCritID++, "Criterion");
  m_critCtrls.push_back(critCtrl);
  m_mainSizer->Insert(0, critCtrl, 1, wxEXPAND);
  m_mainSizer->Layout();
  m_mainSizer->SetSizeHints(this);

  GetSizer()->Layout();
  GetSizer()->SetSizeHints(this);
  m_parent->GetSizer()->Layout();
}

void TemplateDeductionDialog::OnSetDeductionType(wxCommandEvent &e)
{
  if (e.GetId() == ID_SIMPLE)
    SetDeductionType(true);
  else if (e.GetId() == ID_COMPLEX)
    SetDeductionType(false);
}

void TemplateDeductionDialog::OnOK(wxCommandEvent &e)
{
  m_deduction->m_label = m_nameCtrl->GetValue();

  if (!m_simple)
  {
    for (size_t i = 0; i < m_critCtrls.size(); i++)
      m_deduction->AddChoice(m_critCtrls[i]->GetValue().c_str());
  }

  std::stringstream ss(m_mapCtrl->GetValue().c_str());
  std::string item;
  int index = 1;
  while (std::getline(ss, item, ','))
  {
    float value;
    sscanf(item.c_str(), "%f", &value);
    m_deduction->SetMapping(index++, value);
  }

  EndModal(wxID_OK);
}

void TemplateDeductionDialog::OnCancel(wxCommandEvent &e)
{
  EndModal(wxID_CANCEL);
}

void TemplateDeductionDialog::OnAddCriterion(wxCommandEvent &e)
{
  AddCriterion();
}

//-----

IMPLEMENT_CLASS(TemplateCategory, wxPanel)

BEGIN_EVENT_TABLE(TemplateCategory, wxPanel)
  EVT_BUTTON(ID_ADD_DEDUCTION, TemplateCategory::OnAddDeduction)
END_EVENT_TABLE()

TemplateCategory::TemplateCategory(wxWindow *parent, GradingCategory cat):
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
  m_category = cat;
  m_currentID = ID_DEDUCTION;

  wxSizer *szr = new wxBoxSizer(wxVERTICAL);
  SetSizer(szr);

  m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, m_category.m_label);
  szr->Add(m_mainSizer, 0, wxEXPAND | wxALL, 4);

  szr->Add(new wxButton(this, ID_ADD_DEDUCTION, "Add deduction"), 0, wxALIGN_RIGHT | wxALIGN_BOTTOM, 0);

  szr->Layout();
  szr->SetSizeHints(this);
}

GradingCategory &TemplateCategory::GetGradingCategory()
{
  return m_category;
}

void TemplateCategory::AddDeduction()
{
  GradingDeduction d;
  TemplateDeductionDialog(this, &d).ShowModal();

  if (d.m_label == "")
    return;

  m_category.m_dedux.push_back(d);

  //m_mainSizer->Add(new wxStaticBox(this, wxID_ANY, d.m_label), 1, wxEXPAND, 0);
  wxString pts;
  for (size_t i = 0; i < d.m_mapping.size(); i++)
  {
    char buf[32];
    sprintf(buf, "%.2f", d.m_mapping[i]);
    pts = pts + buf;
    if (i < d.m_mapping.size() - 1)
      pts = pts + ", ";
  }
  wxString choices;
  for (size_t i = 0; i < d.m_choices.size(); i++)
    choices += "\n\t" + d.m_choices[i];

  m_mainSizer->Add(new wxStaticText(this, wxID_ANY, pts + ": " + d.m_label + choices), 0, wxEXPAND);
  m_mainSizer->Layout();
  m_mainSizer->SetSizeHints(((wxStaticBoxSizer *)m_mainSizer)->GetStaticBox());

  GetSizer()->Layout();
  GetSizer()->SetSizeHints(this);
  m_parent->GetSizer()->Layout();
}

void TemplateCategory::OnAddDeduction(wxCommandEvent &e)
{
  AddDeduction();
}

//-----

IMPLEMENT_CLASS(TemplateMaker, wxScrolledWindow)

BEGIN_EVENT_TABLE(TemplateMaker, wxScrolledWindow)
  EVT_BUTTON(ID_ADD_CATEGORY, TemplateMaker::OnAddCategory)
END_EVENT_TABLE()

TemplateMaker::TemplateMaker(wxWindow *parent):
  wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
  wxSizer *szr = new wxBoxSizer(wxVERTICAL);
  SetSizer(szr);

  m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Categories");
  szr->Add(m_mainSizer, 0, wxEXPAND | wxALL, 4);

  szr->Add(new wxButton(this, ID_ADD_CATEGORY, "Add category"), 0, wxALIGN_RIGHT | wxALIGN_BOTTOM | wxALL, 8);

  SetScrollbars(0, 10, 0, 100);

  szr->Layout();
  szr->SetSizeHints(this);
}

void TemplateMaker::SaveTemplate()
{
  std::string filename;
  wxFileDialog *f = new wxFileDialog(NULL, "Save grade template", "", "", "All files (*.*)|*.*", wxFD_SAVE);
  if (f->ShowModal() == wxID_OK)
  {
    filename = f->GetPath();
    std::ofstream f(filename.c_str(), std::ios::out);

    for (size_t i = 0; i < m_categories.size(); i++)
      f << m_categories[i]->GetGradingCategory().ToString() << "\n";
  }
  delete f;
}

void TemplateMaker::AddCategory()
{
  GradingCategory cat = CategoryDialog(this);

  if (cat.m_label == "")
    return;

  TemplateCategory *tc = new TemplateCategory(this, cat);
  m_categories.push_back(tc);

  m_mainSizer->Add(tc, 0, wxEXPAND, 0);
  m_mainSizer->Layout();
  m_mainSizer->SetSizeHints(this);

  GetSizer()->Layout();
  m_parent->GetSizer()->Layout();
}

void TemplateMaker::OnAddCategory(wxCommandEvent &e)
{
  AddCategory();
}
