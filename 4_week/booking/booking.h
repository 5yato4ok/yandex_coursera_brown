#include "new"

namespace RAII {
template <class Provider>
class Booking {
 public:
  Booking(Provider* p, int counter) : provider(p), id(counter) {}
  Booking(const Booking&) = delete;
  Booking& operator=(const Booking&) = delete;

  Booking(Booking&& other) {
    provider = other.provider;
    other.provider = nullptr;
    id = other.id;
  }

  Booking& operator=(Booking&& other) {
    if (provider) provider->CancelOrComplete(*this);
    provider = other.provider;
    other.provider = nullptr;
    id = other.id;
    return *this;
  }

  ~Booking() {
    if (provider) provider->CancelOrComplete(*this);
  }

 private:
  Provider* provider;
  int id;
};
}  // namespace RAII