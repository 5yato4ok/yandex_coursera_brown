#include <sstream>

#include "Common.h"
#include "test_runner.h"

using namespace std;

namespace Expr {

class Value : public Expression {
 public:
  Value(const int& val_) : val(val_){};
  int Evaluate() const override { return val; }
  std::string ToString() const override { return std::to_string(val); }

 private:
  int val;
};

class Sum : public Expression {
 public:
  Sum(ExpressionPtr left_, ExpressionPtr right_) : left(std::move(left_)), right(std::move(right_)){};

  int Evaluate() const override { return left->Evaluate() + right->Evaluate(); }

  std::string ToString() const override { return "(" + left->ToString() + ")+(" + right->ToString() + ")"; }

 private:
  ExpressionPtr left;
  ExpressionPtr right;
};

class Product : public Expression {
 public:
  Product(ExpressionPtr left_, ExpressionPtr right_) : left(std::move(left_)), right(std::move(right_)){};

  int Evaluate() const override { return left->Evaluate() * right->Evaluate(); }

  std::string ToString() const override { return "(" + left->ToString() + ")*(" + right->ToString() + ")"; }

 private:
  ExpressionPtr left;
  ExpressionPtr right;
};
}  // namespace Expr

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) { return make_unique<Expr::Sum>(move(left), move(right)); }

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<Expr::Product>(move(left), move(right));
}

ExpressionPtr Value(int value) { return make_unique<Expr::Value>(value); }

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}