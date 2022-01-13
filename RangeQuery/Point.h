#include <cstdint>
#include <array>

typedef uint8_t dim_t;

template<typename T, dim_t d> class Point {
private:
    std::array<T, d> m_dimensions;
public:
    T ElementType = typeid(T);

    static_assert(d != 0, "dimensions can't be zero");
    static constexpr dim_t Dimension = d;

    Point(T dimension = 0) {
        m_dimensions[0] = dimension;
    }

    Point(std::initializer_list<T> dimensions) {
        for (int i = 0; i < dimensions.size()) {
            m_dimensions[i] = dimensions[i];
        }
    }
};