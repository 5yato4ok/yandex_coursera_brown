#include "ini.h"

#include <sstream>
#include <string>
#include <string_view>


namespace Ini {

void LoadSection(string line, Section* section) {
  int posEq = line.find('=');
  auto category = line.substr(0, posEq);
  auto value = line.substr(posEq + 1, line.size() - 1);
  section->insert({category, value});
}

string GetSectionName(string line) {
  if (line.empty() || line[0] != '[') {
    return "";
  }
  return line.substr(1, line.size() - 2);
}

Document Load(istream& input) {
  Document result;
  Section* curSec = nullptr;
  string line;
  while (getline(input, line)) {
    if (line.empty()) {
      continue;
    }
    auto sectionName = GetSectionName(line);
    if (!sectionName.empty()) {
      curSec = &result.AddSection(sectionName);
      continue;
    }

    LoadSection(line, curSec);
  }
  return result;
}

const Section& Document::GetSection(const string& name) const { return sections.at(name); }

Section& Document::AddSection(string name) { return sections[name]; }

size_t Document::SectionCount() const { return sections.size(); }
}  // namespace Ini