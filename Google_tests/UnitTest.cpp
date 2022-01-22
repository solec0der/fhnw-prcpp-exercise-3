#include "gtest/gtest.h"
#include "RangeQuery.h"
#include <algorithm>
#include <vector>
#include <sstream>
#include <random>

using namespace std;

namespace UnitTest {
    /// <summary>
    /// Tests two point vectors of equality.
    /// </summary>
    /// <typeparam name="P">point data type</typeparam>
    /// <param name="p">pair of point vectors</param>
    template<typename P>
    static void test(const RangeQuery<P> &rq, const P &from, const P &to) {
        auto v1 = rq.trivial(from, to);
        sort(v1.begin(), v1.end());

        auto v2 = rq.efficient(from, to);
        sort(v2.begin(), v2.end());

        ASSERT_EQ(v1, v2);
    }

    static default_random_engine engine;

    TEST(RangeQuery, Simple1D) {
        vector<Point1> v({9, 4, 8, 2, 5});
        RangeQuery<Point1> rq(v);

        test(rq, {1}, {7});
        test(rq, {0}, {1});
        test(rq, {9}, {12});
        test(rq, {2}, {8});
        test(rq, {2}, {2});
        test(rq, {4}, {5});
        test(rq, {8}, {8});

        sort(v.begin(), v.end());
        ASSERT_EQ(vector<Point1>({2, 4, 5, 8, 9}), v);
    }

    TEST(RangeQuery, Duplicates1D) {
        vector<Point1> v({9, 4, 8, 2, 5, 9, 4, 8, 2, 5, 9});
        RangeQuery<Point1> rq(v);

        test(rq, {1}, {7});
        test(rq, {0}, {1});
        test(rq, {9}, {12});
        test(rq, {2}, {8});
        test(rq, {2}, {2});
        test(rq, {4}, {5});
        test(rq, {8}, {8});

        sort(v.begin(), v.end());
        ASSERT_EQ(vector<Point1>({2, 2, 4, 4, 5, 5, 8, 8, 9, 9, 9}), v);
    }

    TEST(RangeQuery, Random1D) {
        uniform_int_distribution<Point1::ElementType> coordsRange(-100, +100);

        // create point vector of random length
        uniform_int_distribution<size_t> numPoints(1000, 2000);
        const size_t n = numPoints(engine);
        vector<Point1> v(n);

        // fill in point vector with random points
        for (size_t i = 0; i < n; i++) {
            v[i] = Point1(coordsRange(engine));
        }

        // run k random queries
        RangeQuery<Point1> rq(v);
        const size_t k = v.size() / 2;

        for (size_t i = 0; i < k; i++) {
            auto p1 = coordsRange(engine);
            auto p2 = coordsRange(engine);
            if (p2 < p1) swap(p1, p2);
            test(rq, Point1(p1), Point1(p2));
        }
    }

    TEST(RangeQuery, Simple2D) {
        vector<Point2> v({{4, 6},
                          {1, 5},
                          {2, 7},
                          {3, 8},
                          {1, 1},
                          {2, 5},
                          {6, 1},
                          {4, 4}});
        RangeQuery<Point2> rq(v);

        test(rq, {1, 1}, {7, 7});
        test(rq, {1, 1}, {2, 7});
        test(rq, {1, 1}, {3, 7});
        test(rq, {2, 6}, {3, 7});
        test(rq, {3, 6}, {3, 7});
        test(rq, {4, 6}, {4, 7});
        test(rq, {5, 6}, {5, 8});

        sort(v.begin(), v.end());
        ASSERT_EQ(vector<Point2>({{1, 1},
                                  {1, 5},
                                  {2, 5},
                                  {2, 7},
                                  {3, 8},
                                  {4, 4},
                                  {4, 6},
                                  {6, 1}}), v);
    }

    TEST(RangeQuery, Duplicates2D) {
        vector<Point2> v({{4, 6},
                          {1, 5},
                          {2, 7},
                          {3, 8},
                          {1, 1},
                          {2, 5},
                          {6, 1},
                          {4, 4},
                          {1, 5},
                          {2, 7},
                          {3, 8},
                          {1, 1},
                          {2, 5},
                          {6, 1},
                          {4, 4},
                          {4, 4},
                          {1, 5},
                          {2, 7},
                          {3, 8},
                          {1, 1},
                          {2, 5}});
        RangeQuery<Point2> rq(v);

        test(rq, {1, 1}, {2, 7});
        test(rq, {1, 1}, {7, 7});
        test(rq, {1, 1}, {3, 7});
        test(rq, {2, 6}, {3, 7});
        test(rq, {3, 6}, {3, 7});
        test(rq, {4, 6}, {4, 7});
        test(rq, {5, 6}, {5, 8});

        sort(v.begin(), v.end());
        ASSERT_EQ(vector<Point2>({{1, 1},
                                  {1, 1},
                                  {1, 1},
                                  {1, 5},
                                  {1, 5},
                                  {1, 5},
                                  {2, 5},
                                  {2, 5},
                                  {2, 5},
                                  {2, 7},
                                  {2, 7},
                                  {2, 7},
                                  {3, 8},
                                  {3, 8},
                                  {3, 8},
                                  {4, 4},
                                  {4, 4},
                                  {4, 4},
                                  {4, 6},
                                  {6, 1},
                                  {6, 1}}), v);
    }

    TEST(RangeQuery, Random2D) {
        uniform_int_distribution<Point2::ElementType> coordsRange(-100, +100);

        // create point vector of random length
        uniform_int_distribution<size_t> numPoints(500, 1000);
        const size_t n = numPoints(engine);
        vector<Point2> v(n);

        // fill in point vector with random points
        for (size_t i = 0; i < n; i++) {
            v[i] = Point2({coordsRange(engine), coordsRange(engine)});
        }

        // run k random queries
        RangeQuery<Point2> rq(v);
        const size_t k = v.size() / 2;

        for (size_t i = 0; i < k; i++) {
            auto p1x = coordsRange(engine);
            auto p2x = coordsRange(engine);
            auto p1y = coordsRange(engine);
            auto p2y = coordsRange(engine);
            if (p2x < p1x) swap(p1x, p2x);
            if (p2y < p1y) swap(p1y, p2y);
            test(rq, Point2({p1x, p1y}), Point2({p2x, p2y}));
        }
    }

    TEST(RangeQuery, Simple3D) {
        vector<Point3> v({{4,   6,   4.5},
                          {1,   5,   4},
                          {2.5, 7,   6},
                          {3,   8,   3},
                          {1,   1.5, 5},
                          {2.5, 5.5, 1},
                          {6,   1,   2},
                          {4,   4,   7}});
        RangeQuery<Point3> rq(v);

        test(rq, {1, 1, 1.5}, {7, 7, 3});
        test(rq, {1, 1, 4}, {2, 7, 6});
        test(rq, {1, 1, 1}, {3, 7, 7});
        test(rq, {2, 6, 2}, {3, 7, 4});
        test(rq, {3, 6, 2}, {3, 7, 2});
        test(rq, {4, 5.5, 0}, {4, 7, 8});
        test(rq, {5, 6, 1}, {5, 8, 3});

        sort(v.begin(), v.end());
        ASSERT_EQ(vector<Point3>({{1,   1.5, 5},
                                  {1,   5,   4},
                                  {2.5, 5.5, 1},
                                  {2.5, 7,   6},
                                  {3,   8,   3},
                                  {4,   4,   7},
                                  {4,   6,   4.5},
                                  {6,   1,   2}}), v);
    }

    TEST(RangeQuery, Duplicates3D) {
        vector<Point3> v({{4,   6,   4.5},
                          {1,   5,   4},
                          {2.5, 7,   6},
                          {3,   8,   3},
                          {1,   1.5, 5},
                          {2.5, 5.5, 1},
                          {6,   1,   2},
                          {4,   4,   7},
                          {4,   6,   4.5},
                          {1,   5,   4},
                          {2.5, 7,   6},
                          {3,   8,   3},
                          {1,   1.5, 5},
                          {1,   1.5, 5},
                          {2.5, 5.5, 1},
                          {6,   1,   2},
                          {4,   4,   7},
                          {4,   6,   4.5},
                          {1,   5,   4}});
        RangeQuery<Point3> rq(v);

        test(rq, {1, 1, 1.5}, {7, 7, 3});
        test(rq, {1, 1, 4}, {2, 7, 6});
        test(rq, {1, 1, 1}, {3, 7, 7});
        test(rq, {2, 6, 2}, {3, 7, 4});
        test(rq, {3, 6, 2}, {3, 7, 2});
        test(rq, {4, 5.5, 0}, {4, 7, 8});
        test(rq, {5, 6, 1}, {5, 8, 3});
    }

    TEST(RangeQuery, Random3D) {
        uniform_real_distribution<Point3::ElementType> coordsRange(-100, +100);

        // create point vector of random length
        uniform_int_distribution<size_t> numPoints(333, 666);
        const size_t n = numPoints(engine);
        vector<Point3> v(n);

        // fill in point vector with random points
        for (size_t i = 0; i < n; i++) {
            v[i] = Point3({coordsRange(engine), coordsRange(engine)});
        }

        // run k random queries
        RangeQuery<Point3> rq(v);
        const size_t k = v.size() / 2;

        for (size_t i = 0; i < k; i++) {
            auto p1x = coordsRange(engine);
            auto p2x = coordsRange(engine);
            auto p1y = coordsRange(engine);
            auto p2y = coordsRange(engine);
            auto p1z = coordsRange(engine);
            auto p2z = coordsRange(engine);
            if (p2x < p1x) swap(p1x, p2x);
            if (p2y < p1y) swap(p1y, p2y);
            if (p2z < p1z) swap(p1z, p2z);
            test(rq, Point3({p1x, p1y, p1z}), Point3({p2x, p2y, p2z}));
        }
    }
}
