#include "Common.h"

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`
class Shape : public IShape {
 public:
  void SetPosition(Point pos) override { position = pos; }
  Point GetPosition() const override { return position; }

  void SetSize(Size size_) override { size = size_; }
  Size GetSize() const override { return size; }

  void SetTexture(std::shared_ptr<ITexture> texture_) { texture = std::move(texture_); }
  ITexture* GetTexture() const override { return texture.get(); }

  // Рисует фигуру на указанном изображении
  void Draw(Image& image) const override {
    if (image.empty()) return;

    auto texture_image = texture ? texture->GetImage() : Image(size.height, std::string(size.width, '.'));

    const int n = image.size();
    const int m = image[0].size();

    for (int i = position.y; i < position.y + size.height; ++i) {
      for (int j = position.x; j < position.x + size.width; ++j) {
        if (i >= 0 && i < n && j >= 0 && j < m) {
          const Point point = Point{j - position.x, i - position.y};
          const int i_index = i - position.y;
          const int j_index = j - position.x;

          if (CheckPointInShape(point)) {
            image[i][j] = CheckPointInTexture(point, texture_image) ? texture_image[point.y][point.x] : '.';
          }
        }
      }
    }
  }

 private:
  bool CheckPointInTexture(Point point, const Image& texture_image) const {
    return point.y < texture_image.size() && point.x < texture_image[point.y].size();
  }

  virtual bool CheckPointInShape(Point) const = 0;

  Point position;
  Size size;
  std::shared_ptr<ITexture> texture;
};

class Rectangle : public Shape {
 public:
  std::unique_ptr<IShape> Clone() const override { return std::make_unique<Rectangle>(*this); }

 private:
  bool CheckPointInShape(Point point) const override { return true; }
};

class Ellipse : public Shape {
 public:
  std::unique_ptr<IShape> Clone() const override { return std::make_unique<Ellipse>(*this); }

 private:
  bool CheckPointInShape(Point point) const override { return IsPointInEllipse(point, GetSize()); }
};

std::unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  if (shape_type == ShapeType::Rectangle) return std::make_unique<Rectangle>();
  return std::make_unique<Ellipse>();
}