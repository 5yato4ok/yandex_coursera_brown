#include <algorithm>
#include <deque>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

#include "test_runner.h"
using namespace std;

template <class T>
class ObjectPool {
 public:
  T* Allocate() {
    T* result;
    if (!freePool.empty()) {
      result = allocatedPool.emplace_front(freePool.front());
      freePool.pop_front();
    } else {
      result = allocatedPool.emplace_front(new T());
    }
    return result;
  }
  T* TryAllocate() {
    if (freePool.empty()) {
      return nullptr;
    }
    return Allocate();
  }

  void Deallocate(T* object) {
    auto it = std::find(allocatedPool.begin(), allocatedPool.end(), object);
    if (it == allocatedPool.end()) {
      throw std::invalid_argument("Invalid arg");
    }
    freePool.emplace_back(*it);
    allocatedPool.erase(it);
  }

  ~ObjectPool() {
    auto deleter = [](T* v) { delete v; };
    std::for_each(allocatedPool.begin(), allocatedPool.end(), deleter);
    while (!freePool.empty()) {
      auto x = freePool.front();
      freePool.pop_front();
      delete x;
    }
  }

 private:
  deque<T*> allocatedPool;
  deque<T*> freePool;
  // Добавьте сюда поля
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
