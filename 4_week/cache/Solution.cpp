#include <map>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "Common.h"

using namespace std;

class LruCache : public ICache {
 public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings_)
      : settings(settings_), unpacker(std::move(books_unpacker)), freeMem(settings_.max_memory) {
    counter_ = 0;
  }

  BookPtr GetBook(const string& book_name) override {
    lock_guard<mutex> lock(m);

    auto it = bookToPtr.find(book_name);
    if (it != bookToPtr.end()) {
      return getExistingOne(it);
    }
    return createNewOne(book_name);
  }

 private:
  int counter_;
  using itStorage = map<int, BookPtr>::iterator;

  using itBookPtr = unordered_map<string, itStorage>::iterator;
  using bookToPtrNode = unordered_map<string, itStorage>::node_type;
  map<int, BookPtr> storage;
  unordered_map<string, itStorage> bookToPtr;
  const Settings& settings;
  shared_ptr<IBooksUnpacker> unpacker;
  mutex m;
  int freeMem;

  BookPtr getExistingOne(itBookPtr& existing) {
    BookPtr savedBook = existing->second->second;
    auto map_node = storage.extract(existing->second);
    map_node.key() = counter_++;

    bookToPtr.erase(existing);

    auto result = storage.insert(std::move(map_node));
    if (result.inserted) bookToPtr[savedBook->GetName()] = std::move(result.position);

    return result.inserted ? savedBook : nullptr;
  }

  BookPtr createNewOne(const string& book_name) {
    BookPtr new_book = unpacker->UnpackBook(book_name);
    const auto reqMem = new_book->GetContent().size();

    if (freeMem >= reqMem || checkCache(reqMem)) {
      freeMem -= reqMem;
      auto res = storage.insert({counter_++, new_book});
      if (res.second) {
        bookToPtr[new_book->GetName()] = res.first;
      } else {
        new_book = nullptr;
      }
    }
    return new_book;
  }

  void insertNewBook(BookPtr& book, size_t reqMem) {}

  bool checkCache(int requiredMem) {
    while (!storage.empty()) {
      const auto start = storage.begin();
      freeMem += start->second->GetContent().size();
      bookToPtr.erase(start->second->GetName());
      storage.erase(start);

      if (freeMem >= requiredMem) break;
    }
    return freeMem >= requiredMem;
  }
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker, const ICache::Settings& settings) {
  return make_unique<LruCache>(std::move(books_unpacker), settings);
}
