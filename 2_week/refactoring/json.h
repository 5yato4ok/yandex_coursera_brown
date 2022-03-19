#pragma once

#include <istream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

namespace Json {

class Node {
public:
  explicit Node(const std::vector<Node>& array);
  explicit Node(const std::map<std::string, Node>& map);
  explicit Node(int value);
  explicit Node(const std::string& value);

  const std::vector<Node>& AsArray() const;
  const std::map<std::string, Node>& AsMap() const;
  int AsInt() const;
  const std::string& AsString() const;

private:
  std::vector<Node> as_array;
  std::map<std::string, Node> as_map;
  int as_int;
  std::string as_string;
};

class Document {
public:
  explicit Document(const Node& root);

  const Node& GetRoot() const;

private:
  Node root;
};

Document Load(std::istream& input);

}
