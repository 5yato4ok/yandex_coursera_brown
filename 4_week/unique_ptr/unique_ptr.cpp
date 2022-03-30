#include <cstddef>  // нужно для nullptr_t

#include "test_runner.h"

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
 private:
  T* data;
  void freeMem() {
    delete data;
    data = nullptr;
  }

 public:
  UniquePtr() : data(nullptr){};
  UniquePtr(T* ptr) : data(std::move(ptr)){};
  UniquePtr(const UniquePtr&) = delete;
  UniquePtr(UniquePtr&& other) {
    data = std::move(other.data);
    other.data = nullptr;
  }
  UniquePtr& operator=(const UniquePtr&) = delete;
  UniquePtr& operator=(nullptr_t) {
    freeMem();
    return *this;
  }
  UniquePtr& operator=(UniquePtr&& other) {
    freeMem();
    data = other.data;
    other.data = nullptr;
    return *this;
  }

  ~UniquePtr() { freeMem(); }

  T& operator*() const { return *data; }

  T* operator->() const { return data; }

  T* Release() {
    auto saved = data;
    data = nullptr;
    return saved;
  }

  void Reset(T* ptr) {
    freeMem();
    data = ptr;
  }

  void Swap(UniquePtr& other) {
    auto saved = data;
    data = other.data;
    other.data = saved;
  }

  T* Get() const { return data; }
};

struct Item {
  static int counter;
  int value;
  Item(int v = 0) : value(v) { ++counter; }
  Item(const Item& other) : value(other.value) { ++counter; }
  ~Item() { --counter; }
};

int Item::counter = 0;

void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
}
