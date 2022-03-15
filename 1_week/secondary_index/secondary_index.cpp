#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>

#include "test_runner.h"

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
 public:
  bool Put(const Record& record) {
    if (itersById.find(record.id) != itersById.end()) {
      return false;
    }
    Record* data = new Record(record);
    auto it = dataStorage.insert(dataStorage.begin(), data);
    auto itRecByTime = recordOrderedByTimeStamp.insert({record.timestamp, it});
    auto itRecByKarma = recordOrderedByKarma.insert({record.karma, it});
    auto itRecByUser = recordOrderedByUser.insert({record.user, it});
    if (itRecByTime == recordOrderedByTimeStamp.end() ||
        itRecByKarma == recordOrderedByKarma.end() ||
        itRecByUser == recordOrderedByUser.end()) {
      return false;
    }
    itersById.insert({record.id, {it, itRecByTime, itRecByKarma, itRecByUser}});
    return true;
  }
  const Record* GetById(const string& id) const {
    auto result = itersById.find(id);
    return result == itersById.end() ? nullptr : *result->second.iterData;
  }
  bool Erase(const string& id) {
    auto result = itersById.find(id);
    if (result == itersById.end()) {
      return false;
    }
    
    dataStorage.erase(result->second.iterData);
    auto iter = itersById.at(id);

    recordOrderedByTimeStamp.erase(iter.iterOrderedByTimestamp);
    recordOrderedByKarma.erase(iter.iterOrderedByKarma);
    recordOrderedByUser.erase(iter.iterOrderedByUser);

    itersById.erase(result);

    delete *result->second.iterData;
    return true;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    for (auto it = recordOrderedByTimeStamp.find(low);
         it != recordOrderedByTimeStamp.end(); it++) {
      if (it->first > high) {
        break;
      }
      callback(**it->second);
    }
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    for (auto it = recordOrderedByKarma.find(low);
         it != recordOrderedByKarma.end(); it++) {
      if (it->first > high) {
        break;
      }
      callback(**it->second);
    }
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    for (auto it = recordOrderedByUser.find(user);
         it != recordOrderedByUser.end(); it++) {
      if (it->first != user) {
        break;
      }
      callback(**it->second);
    }
  }

 private:
  using dataIter = std::list<Record*>::iterator;

  struct iters {
    dataIter iterData;
    std::multimap<int, dataIter>::iterator iterOrderedByTimestamp;
    std::multimap<int, dataIter>::iterator iterOrderedByKarma;
    std::multimap<string, dataIter>::iterator iterOrderedByUser;
  };

  std::list<Record*> dataStorage;
  std::unordered_map<string, iters> itersById;

  std::multimap<int, dataIter> recordOrderedByTimeStamp;
  std::multimap<int, dataIter> recordOrderedByKarma;
  std::multimap<string, dataIter> recordOrderedByUser;
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}
