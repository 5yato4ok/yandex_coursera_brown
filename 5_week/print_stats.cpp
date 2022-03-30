#include <iostream>
#include <vector>
#include <string>

void PrintCategory(const std::string& category_name, int val) { std::cout << category_name << " = " << val << "\n"; }

void PrintStats(std::vector<Person> persons) {
  // все люди;
  PrintCategory("Median age", ComputeMedianAge(persons.begin(), persons.end()));

  std::vector<Person> wooman;
  std::vector<Person> man;
  std::vector<Person> employed_wooman;
  std::vector<Person> employed_man;
  std::vector<Person> unemployed_wooman;
  std::vector<Person> unemployed_man;

  for (auto& person : persons) {
    if (person.gender == Gender::MALE) {
      man.push_back(person);
      if (person.is_employed) {
        employed_man.push_back(person);
      } else {
        unemployed_man.push_back(person);
      }

    } else {
      wooman.push_back(person);
      if (person.is_employed) {
        employed_wooman.push_back(person);
      } else {
        unemployed_wooman.push_back(person);
      }
    }
  }
  PrintCategory("Median age for females", ComputeMedianAge( wooman.begin(), wooman.end()));
  PrintCategory("Median age for males", ComputeMedianAge( man.begin(), man.end()));
  PrintCategory("Median age for employed females", ComputeMedianAge( employed_wooman.begin(), employed_wooman.end()));
  PrintCategory("Median age for unemployed females", ComputeMedianAge( unemployed_wooman.begin(), unemployed_wooman.end()));
  PrintCategory("Median age for employed males", ComputeMedianAge( employed_man.begin(), employed_man.end()));
  PrintCategory("Median age for unemployed males", ComputeMedianAge( unemployed_man.begin(), unemployed_man.end()));


}
