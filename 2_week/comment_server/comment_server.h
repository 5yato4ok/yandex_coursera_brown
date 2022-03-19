
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

enum class HttpCode {
  Ok = 200,
  NotFound = 404,
  Found = 302,
};

class HttpResponse {
 public:
  explicit HttpResponse(HttpCode code_) : code(code_){};

  HttpResponse& AddHeader(std::string name, std::string value) {
    headers.push_back({std::move(name), std::move(value)});
    return (*this);
  }

  HttpResponse& SetContent(std::string content) {
    a_content = std::move(content);
    return (*this);
  }

  HttpResponse& SetCode(HttpCode a_code) {
    code = std::move(a_code);
    return (*this);
  }

  friend std::ostream& operator<<(std::ostream& output, const HttpResponse& resp) {
    output << "HTTP/1.1 " << int(resp.code) << " " << resp.comment_map_.at(resp.code) << "\n";
    for (auto& [name, value] : resp.headers) {
      output << name << ": " << value << "\n";
    }
    if (!resp.a_content.empty()) {
      output << "Content-Length: " << resp.a_content.size() << "\n";
      output << "\n" << resp.a_content;
    } else {
      output << "\n";
    }
    return output;
  }

  inline const static std::unordered_map<const HttpCode, std::string> comment_map_ = {
      {HttpCode::Ok, "OK"}, {HttpCode::Found, "Found"}, {HttpCode::NotFound, "Not found"}};
  std::vector<std::pair<std::string, std::string>> headers;
  std::string a_content;
  HttpCode code;
};