#include <cstdint>
#include <array>
#include <cmath>
#include <ostream>

typedef uint8_t dim_t;

template<typename T, dim_t d>
class Point : public std::array<T, d> {
public:
    typedef T ElementType;

    static_assert(d != 0, "dimensions can't be zero");
    static constexpr dim_t Dimension = d;

    Point(T dimension = 0) {
        (*this)[0] = dimension;
    }

    Point(std::initializer_list<T> dimensions) {
        std::copy(dimensions.begin(), dimensions.end(), this->begin());
    }

    Point nextAfter() const {
        std::initializer_list<T> newDimensions;
        Point<T, d> point(newDimensions);

        for (int i = 0; i < (*this).size(); i++) {
            if (std::numeric_limits<T>::is_integer) {
                point[i] = (*this)[i] + 1;
            } else {
                point[i] = std::nextafter((*this)[i], std::numeric_limits<T>::max());
            }
        }
        return point;
    }

    bool operator==(const Point &rhs) const {
        if (this->Dimension != rhs.Dimension) return false;

        for (int i = 0; i < this->size(); i++) {
            if ((*this)[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator<(const Point &rhs) const {
        return std::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end());
    }

    bool operator<=(const Point &rhs) const {
        int i = 0;
        while (i < this->size() && (*this)[i] <= rhs[i]) i++;

        return i == this->size();
    }

    bool operator>=(const Point &rhs) const {
        int i = 0;
        while (i < this->size() && (*this)[i] >= rhs[i]) i++;

        return i == this->size();
    }

    friend std::ostream &operator<<(std::ostream &os, const Point &point) {
        os << "(";
        std::string separator;
        for (const auto &dim: point) {
            os << separator << dim;
            separator = ", ";
        }

        return os << ")";
    }
};

typedef Point<int, 1> Point1;
typedef Point<int, 2> Point2;
typedef Point<double, 3> Point3;

