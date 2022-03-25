#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "test_runner.h"

using namespace std;

struct Email {
  string from;
  string to;
  string body;
};

std::istream& operator>>(std::istream& input, Email& email) {
  std::string from, to, body;

  std::getline(input, from);
  std::getline(input, to);
  std::getline(input, body);

  email = Email{std::move(from), std::move(to), std::move(body)};

  return input;
}

std::ostream& operator<<(std::ostream& output, const Email& email) {
  output << email.from << '\n';
  output << email.to << '\n';
  output << email.body << '\n';

  return output;
}

class Worker {
 public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

 protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const {
    if (next) {
      next->Process(std::move(email));
    }
  }

 public:
  void SetNext(unique_ptr<Worker> next_) {
    if (!next) {
      next = std::move(next_);
    } else {
      next->SetNext(std::move(next_));
    }
  }
  unique_ptr<Worker> next = nullptr;
};

class Reader : public Worker {
 public:
  Reader(istream& in_) : in(in_) {
    Email email;
    while (in >> email) {
      emails.push_back(std::make_unique<Email>(std::move(email)));
    }
  }
  void Run() override {
    for (std::unique_ptr<Email>& email : emails) {
      PassOn(std::move(email));
    }
  }
  void Process(std::unique_ptr<Email> email) override { Run(); }

 private:
  istream& in;
  std::vector<unique_ptr<Email>> emails;
};

class Filter : public Worker {
 public:
  using Function = function<bool(const Email&)>;

 public:
  Filter(const Function& filter) : cb(filter){};
  void Process(unique_ptr<Email> email) override {
    if (cb(*email)) {
      PassOn(std::move(email));
    }
  }

 private:
  Function cb;
};

class Copier : public Worker {
 public:
  Copier(const std::string& address) : copyTo(address) {}
  void Process(unique_ptr<Email> email) override {
    unique_ptr<Email> new_email =
        email->to != copyTo ? make_unique<Email>(Email{email->from, copyTo, email->body}) : nullptr;
    PassOn(std::move(email));
    if (new_email) PassOn(std::move(new_email));
  }

 private:
  std::string copyTo;
};

class Sender : public Worker {
 public:
  Sender(ostream& out_) : out(out_){};
  void Process(unique_ptr<Email> email) override {
    out << *email;
    PassOn(std::move(email));
  }

 private:
  ostream& out;
};

// реализуйте класс
class PipelineBuilder {
 public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in) : head(make_unique<Reader>(in)) {}

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter) {
    head->SetNext(make_unique<Filter>(filter));
    return (*this);
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient) {
    head->SetNext(make_unique<Copier>(recipient));
    return (*this);
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out) {
    head->SetNext(make_unique<Sender>(out));
    return (*this);
  }

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build() { return std::move(head); }

 private:
  unique_ptr<Worker> head;
};

void TestSanity() {
  string input =
      ("erich@example.com\n"
       "richard@example.com\n"
       "Hello there\n"

       "erich@example.com\n"
       "ralph@example.com\n"
       "Are you sure you pressed the right button?\n"

       "ralph@example.com\n"
       "erich@example.com\n"
       "I do not make mistakes of that kind\n");
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) { return email.from == "erich@example.com"; });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput =
      ("erich@example.com\n"
       "richard@example.com\n"
       "Hello there\n"

       "erich@example.com\n"
       "ralph@example.com\n"
       "Are you sure you pressed the right button?\n"

       "erich@example.com\n"
       "richard@example.com\n"
       "Are you sure you pressed the right button?\n");

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
