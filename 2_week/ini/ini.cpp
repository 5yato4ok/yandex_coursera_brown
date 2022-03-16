#include "ini.h"

#include <sstream>
#include <string>

namespace Ini {

void LoadSection(string line, Section* section) {
  int posEq = line.find('=');
  auto category = line.substr(0, posEq);
  auto value = line.substr(posEq + 1, line.size() - 1);
  section->insert({category, value});
}

string GetSectionName(const string& line) {
  if (line.empty() || line[0] != '[') {
    return "";
  }
  return line.substr(1, line.find(']') - 1);
}

Document Load(istream& input) {
  Document result;
  string line;
  getline(input, line);
  Section* curSec = &result.AddSection(GetSectionName(line));
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

const Section& Document::GetSection(const string& name) const {
  auto test = sections.at(name);
  return sections.at(name);
}

Section& Document::AddSection(string name) { return sections[name]; }

size_t Document::SectionCount() const { return sections.size(); }
}  // namespace Ini