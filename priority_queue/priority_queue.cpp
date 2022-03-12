#include "test_runner.h"
#include "profile.h"

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <iostream>

using namespace std;

template <typename T>
class PriorityCollection {
public:
  using Id = int;
  using PriorityDesc = pair<int, Id>;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
    auto index = idx++;
    auto desc = PriorityDesc(0, index);
    priority.insert(desc);
    data[index] = make_pair(move(object),0);
    return index;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end,
           IdOutputIt ids_begin) {
      for(auto it = range_begin;it!=range_end;++it) {
        *(ids_begin++) = Add(move(*it));
      }
  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
    return data.find(id) != data.end();
  }

  // Получить объект по идентификатору
  const T& Get(Id id) const {
    return data.at(id).first;
  }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) { 
      auto curPriority = data[id].second++;
      priority.extract({curPriority,id});
      priority.insert({curPriority+1, id});
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
    return { data.at(prev(priority.end())->second).first,  prev(priority.end())->first };
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
    auto priority_rate = prev(priority.end());
    Id max_id = prev(priority.end())->second;
    priority.erase(priority_rate);
    auto max_data = data.extract(max_id);
    return make_pair( std::move(max_data.mapped().first),max_data.mapped().second);
  }

private:
  Id idx = 0;
  set<PriorityDesc> priority;
  unordered_map<Id, std::pair<T,int>> data;
};


// class StringNonCopyable : public string {
// public:
//     using string::string;  // Позволяет использовать конструкторы строки
//     StringNonCopyable(const StringNonCopyable &) = delete;

//     StringNonCopyable(StringNonCopyable &&) = default;

//     StringNonCopyable &operator=(const StringNonCopyable &) = delete;

//     StringNonCopyable &operator=(StringNonCopyable &&) = default;
// };

// void TestNoCopy() {
//     PriorityCollection<StringNonCopyable> strings;

//     const auto white_id = strings.Add("white");
//     const auto yellow_id = strings.Add("yellow");
//     const auto red_id = strings.Add("red");

//     strings.Promote(yellow_id);
//     for (int i = 0; i < 2; ++i) {
//         strings.Promote(red_id);
//     }
//     strings.Promote(yellow_id);
//     {
//         const auto item = strings.PopMax();
//         ASSERT_EQUAL(item.first, "red");
//         ASSERT_EQUAL(item.second, 2);
//     }
//     {

//         const auto item = strings.PopMax();
//         ASSERT_EQUAL(item.first, "yellow");
//         ASSERT_EQUAL(item.second, 2);
//     }
//     {
//         const auto item = strings.PopMax();
//         ASSERT_EQUAL(item.first, "white");
//         ASSERT_EQUAL(item.second, 0);
//     }
// }

// int main() {
//     TestRunner tr;
//     RUN_TEST(tr, TestNoCopy);
//     return 0;
// }