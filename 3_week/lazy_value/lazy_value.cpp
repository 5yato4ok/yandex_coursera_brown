#include "test_runner.h"

#include <functional>
#include <string>
#include <optional>
#include <memory>
using namespace std;

template <typename T>
class LazyValue {
public:
  using init_func = std::function<T()>;
  explicit LazyValue(const init_func& init_):init(init_){}

  bool HasValue() const {
    return data.has_value();
  }

  const T& Get() const {
    if (!data) {
      data = init();
    }
    return *data;
  }

private:
init_func init;
mutable std::optional<T> data;
};

void UseExample() {
  const string big_string = "Giant amounts of memory";

  LazyValue<string> lazy_string([&big_string] { return big_string; });

  ASSERT(!lazy_string.HasValue());
  ASSERT_EQUAL(lazy_string.Get(), big_string);
  ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
  bool called = false;

  {
    LazyValue<int> lazy_int([&called] {
      called = true;
      return 0;
    });
  }
  ASSERT(!called);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, UseExample);
  RUN_TEST(tr, TestInitializerIsntCalled);
  return 0;
}
