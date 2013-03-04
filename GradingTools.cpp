
#include "GradingTools.h"

#include "wx/dir.h"
#include <sstream>
#include <fstream>

// You don't have to free the pointer returned by this, and you shouldn't.
char *formatFloat(float x)
{
  // Clearly, just making this static could cause problems.
  // But it won't.
  static char s[32];

  if (fabsf(x) - (int)fabsf(x) < 0.01f)
    sprintf(s, "%d", (int)x);
  else if (fabsf(x) - (int)fabsf(x) < 0.1f)
    sprintf(s, "%.2f", x);
  else
    sprintf(s, "%.1f", x);

  return s;
}

//-----

GradingDeduction::GradingDeduction()
{
  m_label = "";
  m_recorded = 0.0f;
}

GradingDeduction::GradingDeduction(std::string label)
{
  SetLabel(label);
  m_recorded = 0.0f;
}

GradingDeduction::GradingDeduction(const GradingDeduction &d)
{
  *this = d;
}

GradingDeduction &GradingDeduction::operator=(const GradingDeduction &d)
{
  m_label = d.m_label;
  m_mapping = d.m_mapping;
  m_choices = d.m_choices;
  m_recorded = d.m_recorded;

  return *this;
}

void GradingDeduction::SetLabel(std::string label)
{
  // Vestigial template-generalizing code
/*
  if (label.find(":") != std::string::npos)
  {
    int ind = 0;
    if ((ind = label.find(" or ")) != std::string::npos)
      label.erase(ind, label.find_first_of(" ", ind + 4) - ind);
    if ((ind = label.find_first_of("123456789")) != std::string::npos)
      label[ind] = '#';
  }
*/
  m_label = label;
}

void GradingDeduction::SetCheckbox(int index, bool state)
{
  m_checkboxes[index]->SetValue(state);
}

void GradingDeduction::SetMapping(int index, float value)
{
  index--;

  if ((unsigned int)index >= m_mapping.size())
    m_mapping.resize(index + 1, (m_mapping.size() > 0)?m_mapping[m_mapping.size() - 1]:0.0f);

  m_mapping[index] = value;
}

float GradingDeduction::GetValue()
{
  int t = 0;

  for (unsigned int i = 0; i < m_checkboxes.size(); i++)
    if (m_checkboxes[i]->GetValue())
      t++;

  if (t == 0)
    return 0.0f;

  t--;

  if (m_mapping.size() <= t)
    return m_mapping[m_mapping.size() - 1];

  return m_mapping[t];
}

float GradingDeduction::GetMaxValue()
{
  return m_mapping[m_mapping.size() - 1];
}

void GradingDeduction::UpdateTotal(float *total)
{
  *total -= m_recorded;
  *total += (m_recorded = GetValue());
}

void GradingDeduction::AddChoice(std::string label)
{
  m_choices.push_back(label);
}

std::string GradingDeduction::Print()
{
  std::stringstream s;

  if (GetValue() == 0.0f)
    return "";

  if (m_choices.size() == 0)
    s << "  " << formatFloat(GetValue()) << " " << m_label << '\n';
  else
  {
    std::string label = m_label;
    int t = 0, ind = 0;
    for (unsigned int i = 0; i < m_checkboxes.size(); i++)
      if (m_checkboxes[i]->GetValue())
        t++;
    //if (t == 1 && label[ind = (label.find(":") - 1)] == 's')
    //  label.erase(ind, 1);
    //label[label.find_first_of("#")] = (char)(t + '0');
    s << "  " << formatFloat(GetValue()) << " " << label << '\n';
    for (unsigned int i = 0; i < m_checkboxes.size(); i++)
      if (m_checkboxes[i]->GetValue())
        s << "    " << m_choices[i] << '\n';
  }

  return s.str();
}

void GradingDeduction::Reset()
{
  for (unsigned int i = 0; i < m_checkboxes.size(); i++)
    m_checkboxes[i]->SetValue(false);
  m_recorded = 0.0f;
}

wxSizer *GradingDeduction::BuildPanel(wxWindow *parent, int *currentID)
{
  char buffer[512];
  wxBoxSizer *sizer;

  if (m_choices.size() > 0)
  {
    sprintf(buffer, "%s", m_label.c_str());
    sizer = new wxStaticBoxSizer(wxVERTICAL, parent, buffer);
    for (unsigned int i = 0; i < m_choices.size(); i++)
    {
      wxCheckBox *check = new wxCheckBox(parent, (*currentID)++, m_choices[i].c_str(), wxDefaultPosition, wxDefaultSize);
      check->SetValue(false);
      sizer->Add(check, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
      m_checkboxes.push_back(check);
    }
  }
  else
  {
    sizer = new wxBoxSizer(wxVERTICAL);

    sprintf(buffer, "%s %s", formatFloat(m_mapping[0]), m_label.c_str());
    wxCheckBox *check = new wxCheckBox(parent, (*currentID)++, buffer, wxDefaultPosition, wxDefaultSize);
    check->SetValue(false);
    sizer->Add(check, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
    m_checkboxes.push_back(check);
  }

  sizer->Layout();

  return sizer;
}

std::string GradingDeduction::ToString()
{
  std::stringstream str;
  str << "\tDED ";

  if (m_choices.size() == 0 && m_checkboxes.size() > 0 && m_checkboxes[0]->GetValue())
    str << "[X] [";
  else
    str << "[O] [";

  for (size_t i = 0; i < m_mapping.size(); i++)
    str << m_mapping[i] << ((i < m_mapping.size() - 1)?", ":"");
  str << "] " << m_label;

  for (size_t i = 0; i < m_choices.size(); i++)
    str << "\n\t\tCRT " << (m_checkboxes.size() > i && m_checkboxes[i]->GetValue()?"[X] ":"[O] ") << m_choices[i];

  return str.str();
}

//-----

GradingCategory::GradingCategory()
{
  m_value = 0.0f;
  m_label = "";
}

GradingCategory::GradingCategory(float value, std::string label, std::vector<GradingDeduction> dedux)
{
  m_value = value;
  m_label = label;
  m_dedux = dedux;
}

GradingCategory::GradingCategory(const GradingCategory &c)
{
  *this = c;
}

GradingCategory &GradingCategory::operator=(const GradingCategory &c)
{
  m_value = c.m_value;
  m_label = c.m_label;
  m_dedux = c.m_dedux;

  return *this;
}

void GradingCategory::UpdateTotal(float *total)
{
  for (size_t i = 0; i < m_dedux.size(); i++)
    m_dedux[i].UpdateTotal(total);
}

void GradingCategory::AddDeduction(GradingDeduction d)
{
  m_dedux.push_back(d);
}

void GradingCategory::SetDeductionBox(int deduction, int box, bool state)
{
  m_dedux[deduction].SetCheckbox(box, state);
}

void GradingCategory::Reset()
{
  for (unsigned int i = 0; i < m_dedux.size(); i++)
    m_dedux[i].Reset();
}

std::string GradingCategory::ToString()
{
  std::stringstream str;
  str << "CAT [" << m_value << "] " << m_label;
  for (size_t i = 0; i < m_dedux.size(); i++)
    str << "\n" << m_dedux[i].ToString();

  return str.str();
}

//-----

IMPLEMENT_CLASS(GradingPanel, wxScrolledWindow)

BEGIN_EVENT_TABLE(GradingPanel, wxScrolledWindow)
  EVT_COMMAND_RANGE(ID_DEDUCTION, ID_DEDUCTION + 99, wxEVT_COMMAND_CHECKBOX_CLICKED, GradingPanel::OnDeduction)
END_EVENT_TABLE()

GradingPanel::GradingPanel(wxWindow* parent, float *total):
  wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
  m_currentID = ID_DEDUCTION;

  m_total = total;

  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

  // Points counter
  m_pointsText = new wxStaticText(this, wxID_ANY, "Total score:\t0");
  topSizer->Add(m_pointsText, 0, wxGROW | wxALL, 2);

  // Deductions
  wxStaticBoxSizer *sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Deductions");
  topSizer->Add(sizer, 0, wxGROW | wxALIGN_CENTER | wxALL, 2);
  m_deduxBox = sizer;

  // Notes
  sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Notes");
  sizer->SetMinSize(0, 150);
  m_notesText = new wxRichTextCtrl(this);

  sizer->Add(m_notesText, 1, wxGROW | wxALL, 2);
  sizer->Layout();
  topSizer->Add(sizer, 1, wxGROW | wxALIGN_CENTER | wxALL, 2);

  this->SetSizer(topSizer);
  topSizer->Layout();
  topSizer->SetSizeHints(this);

  SetScrollbars(0, 10, 0, 100);

  Show(true);
}

GradingPanel::~GradingPanel()
{
}

void GradingPanel::OnDeduction(wxCommandEvent &e)
{
  //((GradingTools *)m_parent)->SetDeductionValue(e.GetId() - ID_DEDUCTION, e.IsChecked());
  m_deduxMapping[e.GetId() - ID_DEDUCTION]->UpdateTotal(m_total);
  SetPoints(*m_total);
}

void GradingPanel::AddCategory(GradingCategory &cat)
{
  char buffer[512];

  sprintf(buffer, "%s %s", formatFloat(cat.m_value), cat.m_label.c_str());
  wxStaticBoxSizer *sizer = new wxStaticBoxSizer(wxVERTICAL, this, buffer);

  for (unsigned int i = 0; i < cat.m_dedux.size(); i++)
  {
    sizer->Add(cat.m_dedux[i].BuildPanel(this, &m_currentID), 0, wxGROW | wxALL | wxALIGN_CENTER_VERTICAL, 2);

    while (m_deduxMapping.size() < (m_currentID - ID_DEDUCTION))
      m_deduxMapping.push_back(&cat.m_dedux[i]);
  }

  m_deduxBox->Add(sizer, 0, wxGROW | wxALL, 6);
  m_deduxBox->Layout();
  m_deduxBox->SetSizeHints(m_parent);

  GetSizer()->Layout();
  GetSizer()->SetSizeHints(this);
}

void GradingPanel::SetPoints(float points)
{
  char buffer[512];
  sprintf(buffer, "Total score: %s", formatFloat(points));

  m_pointsText->SetLabel(buffer);
}

std::string GradingPanel::GetNotes()
{
  return m_notesText->GetValue().c_str();
}

void GradingPanel::SetNotes(std::string notes)
{
  m_notesText->SetValue(notes);
}

void GradingPanel::Reset()
{
  m_notesText->SetValue("");
  m_deduxBox->Clear(true);
  m_deduxMapping.clear();
  m_currentID = ID_DEDUCTION;
}

//-----

IMPLEMENT_CLASS(GradingText, wxRichTextCtrl)

BEGIN_EVENT_TABLE(GradingText, wxRichTextCtrl)
END_EVENT_TABLE()

GradingText::GradingText(std::string filename, wxWindow* parent):
  wxRichTextCtrl(parent)
{
  SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  Load(filename);
}

GradingText::~GradingText()
{
}

void GradingText::Load(std::string filename)
{
  m_filename = filename;

  FILE *f = fopen(filename.c_str(), "r");
  if (f == NULL)
  {
    wxMessageBox("I failed to open a file I was expecting to be able to open. What's the deal with that?", "Oops.", wxOK, m_parent);
    return;
  }
  fpos_t len;

  fseek(f, 0, SEEK_END);
  fgetpos(f, &len);
  fseek(f, 0, SEEK_SET);

  char *content = (char *)malloc(sizeof(char) * len);
  fread(content, len, sizeof(char), f);

  ChangeValue(wxString(content));

  free(content);
  fclose(f);
}

void GradingText::Save()
{
}

//-----

IMPLEMENT_CLASS(GradingTools, wxPanel)

BEGIN_EVENT_TABLE(GradingTools, wxPanel)
END_EVENT_TABLE()

GradingTools::GradingTools(int part, wxWindow *parent, std::string templateFilename):
  wxPanel(parent),
  m_templateFilename(templateFilename)
{
  m_part = part;
  m_partString = (m_part == 1)?"I":((m_part == 2)?"II-1":"II-2");
  m_totalPoints = 0;

  m_panel = new GradingPanel(this, &m_totalPoints);
  m_notebook = new wxNotebook(this, wxID_ANY);

  wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);

  topSizer->Add(m_notebook, 1, wxGROW);
  topSizer->Add(m_panel, 1, wxGROW);

  this->SetSizer(topSizer);
  topSizer->Layout();
  topSizer->SetSizeHints(this);

  OpenFiles(true);
}

GradingTools::~GradingTools()
{
}

void GradingTools::LoadScoreSheet(std::string filename, bool build)
{
  FILE *f = fopen(filename.c_str(), "r");
  if (f == NULL)
  {
    wxMessageBox("I failed to open a file I was expecting to be able to open. What's the deal with that?", "Oops.", wxOK, this);
    return;
  }

  fpos_t len;

  fseek(f, 0, SEEK_END);
  fgetpos(f, &len);
  fseek(f, 0, SEEK_SET);

  char *content = (char *)malloc(sizeof(char) * len);
  memset(content, 0, sizeof(char) * len);
  fread(content, len, sizeof(char), f);

  ParseScoreSheet(content);

  free(content);
  fclose(f);
}

void GradingTools::SaveScoreSheet()
{
  FILE *f = fopen(m_filename.c_str(), "w+");

  char buffer[512];

  sprintf(buffer, "Part %s (%s points total)", m_partString.c_str(), formatFloat(m_maxPoints));
  fprintf(f, "\n%s\n", buffer);

  for (unsigned int i = 0; i < strlen(buffer); i++)
    fprintf(f, "-");
  fprintf(f, "\n");

  // Special case for no submission
  if (m_categories[m_categories.size() - 1].m_dedux[0].GetValue() != 0.0f)
    fprintf(f, "%s %s\n\n", formatFloat(-m_maxPoints), m_categories[m_categories.size() - 1].m_dedux[0].m_label.c_str());

  for (unsigned int i = 0; i < m_categories.size() - 1; i++)
  {
    fprintf(f, "%s %s\n", formatFloat(m_categories[i].m_value), m_categories[i].m_label.c_str());
    for (unsigned int j = 0; j < m_categories[i].m_dedux.size(); j++)
      fprintf(f, "%s", m_categories[i].m_dedux[j].Print().c_str());
    fprintf(f, "\n");
  }

  if (m_panel->GetNotes().length() > 0)
    fprintf(f, "Note: %s\n\n", m_panel->GetNotes().c_str());

  fprintf(f, "late penalty for part %s (if any): \n", m_partString.c_str());
  fprintf(f, "total for part %s: %s\n", m_partString.c_str(), formatFloat(m_totalPoints));

  fprintf(f, "\nTotal: \n");

  fclose(f);

  SaveScoreFile();
}

void GradingTools::SaveScoreFile()
{
  std::ofstream f((m_filename.substr(0, m_filename.find_last_of('.')) + ".ss").c_str(), std::ios::out);

  for (size_t i = 0; i < m_categories.size() - 1; i++)
    f << m_categories[i].ToString() << "\n";

  f << "\nNOTES\n" << m_panel->GetNotes();
}

void GradingTools::OpenFiles(bool build)
{
  m_texts.clear();
  m_notebook->DeleteAllPages();

  wxDir dir(wxGetCwd());

  if (!dir.IsOpened())
  {
    wxMessageBox("I choked on something while trying to open the student's directory. Sorry.", "Uh oh!", wxOK, this);
    return;
  }

  wxString filename, filter;

  if (m_part == 1)
    filter = "ps1_partI.txt";
  else
    filter = "*.java";

  bool more = dir.GetFirst(&filename, filter, wxDIR_FILES);
  while (more)
  {
    m_texts.push_back(new GradingText(filename.c_str(), m_notebook));
    m_notebook->AddPage(m_texts[m_texts.size() - 1], m_texts[m_texts.size() - 1]->m_filename, true);
    more = dir.GetNext(&filename);
  }

  // Look for the official grade file
  if (m_part == 1)
    filter = "*-a.txt";
  else if (m_part == 2)
    filter = "*-b.txt";
  else
    filter = "*-c.txt";
  if (dir.GetFirst(&filename, filter, wxDIR_FILES))
    m_filename = filename;
  else
    wxMessageBox("I couldn't find the grade file! I think something is horribly wrong.", "What.", wxOK, this);

  // Look for the score sheet generated by this program
  filter = "*.ss";
  if (dir.GetFirst(&filename, filter, wxDIR_FILES))
    LoadScoreSheet(filename.c_str(), build);
  else
    LoadScoreSheet(m_templateFilename);

  if (m_texts.size() == 0)
  {
    m_notebook->AddPage(new wxStaticText(this, wxID_ANY, "I couldn't find any texts I thought would be useful. Try looking for yourself, maybe."), "Just a moment!", true);
  }
}

struct sCheckboxIndex {int cat, ded, crt;};
void GradingTools::ParseScoreSheet(std::string content)
{
  std::stringstream sheet(content);
  std::string line;
  GradingCategory *cat = NULL;
  GradingDeduction *ded = NULL;

  std::vector<sCheckboxIndex> onBoxes;
  sCheckboxIndex cBox = {-1, -1, -1};

  while (sheet.good())
  {
    getline(sheet, line);

    // Category
    if (line[0] == 'C')
    {
      cBox.cat++;
      cBox.ded = cBox.crt = -1;
      if (cat != NULL)
      {
        if (ded != NULL)
          cat->AddDeduction(*ded);
        m_categories.push_back(*cat);
      }

      float value;
      sscanf(line.c_str(), "CAT [%f]", &value);
      std::string label(&line[line.find_last_of(']')] + 2);
      cat = new GradingCategory(value, label, std::vector<GradingDeduction>());
      ded = NULL;
    }
    // Deduction
    else if (line[line.find_first_not_of('\t')] == 'D')
    {
      cBox.ded++;
      cBox.crt = -1;
      if (ded != NULL)
        cat->AddDeduction(*ded);

      // If this deduction is simple and applied, remember to check its box later
      if (line[line.find('[') + 1] == 'X')
      {
        cBox.crt = 0;
        onBoxes.push_back(cBox);
      }

      ded = new GradingDeduction();
      std::stringstream points(line.substr(line.find_last_of('[') + 1, line.find_last_of(']') - line.find_last_of('[') - 1));
      std::string item;
      int index = 1;
      while (std::getline(points, item, ','))
      {
        float value;
        sscanf(item.c_str(), "%f", &value);
        ded->SetMapping(index++, value);
      }
      std::string label(&line[line.find_last_of(']')] + 2);
      ded->SetLabel(label);
    }
    // Criterion
    else if (line[line.find_first_not_of('\t')] == 'C')
    {
      cBox.crt++;
      std::string label(&line[line.find_last_of(']')] + 2);
      ded->AddChoice(label);

      // If this criterion is checked, remember to make it so later
      if (line[line.find('[') + 1] == 'X')
        onBoxes.push_back(cBox);
    }
    // Notes
    else if (line[line.find_first_not_of('\t')] == 'N')
    {
      std::string notes;
      while (sheet.good())
      {
        getline(sheet, line);
        notes += line + '\n';
      }
      m_panel->SetNotes(notes);
    }
  }
  if (cat != NULL)
  {
    if (ded != NULL)
      cat->AddDeduction(*ded);
    m_categories.push_back(*cat);
  }

  m_maxPoints = 0;
  for (size_t i = 0; i < m_categories.size(); i++)
    m_maxPoints += m_categories[i].m_value;

  ded = new GradingDeduction("no submission");
  ded->SetMapping(1, -m_maxPoints);
  m_categories.push_back(GradingCategory(m_maxPoints, "points total", std::vector<GradingDeduction>(1, *ded)));

  for (unsigned int i = 0; i < m_categories.size(); i++)
    m_panel->AddCategory(m_categories[i]);

  m_totalPoints = m_maxPoints;
  m_panel->SetPoints(m_maxPoints);

  for (size_t i = 0; i < onBoxes.size(); i++)
    SetDeductionBox(onBoxes[i].cat, onBoxes[i].ded, onBoxes[i].crt, true);
  for (size_t i = 0; i < m_categories.size(); i++)
    m_categories[i].UpdateTotal(&m_totalPoints);

  m_panel->SetPoints(m_totalPoints);
}

void GradingTools::SetDeductionBox(int category, int deduction, int box, bool state)
{
  m_categories[category].SetDeductionBox(deduction, box, state);
}

void GradingTools::UpdateDirectory()
{
  m_totalPoints = 0;

  m_categories.clear();
  m_panel->Reset();

  OpenFiles();

  GetSizer()->Layout();
  GetSizer()->SetSizeHints(m_parent);
}
