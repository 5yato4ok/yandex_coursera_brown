#include <functional>

using CoordType = int;

struct Point3D
{
    CoordType x;
    CoordType y;
    CoordType z;

    bool operator==(const Point3D &other) const
    {
        return other.x == x && other.y == y && other.z == z;
    }
};

struct Hasher
{
    size_t operator()(const Point3D &point) const
    {
        int x = 2'946'901;
        auto a = hasher(point.x);
        auto b = hasher(point.y);
        auto c = hasher(point.z);
        return (a * x * x + b * x + c);
    }
    std::hash<CoordType> hasher;
    // реализуйте структуру
};