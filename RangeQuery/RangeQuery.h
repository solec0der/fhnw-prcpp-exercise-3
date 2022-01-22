//
// Author: Yannick Huggler
//

#include "RangeTree.hpp"
#include "Stopwatch.h"

template<class P>
class RangeQuery {
    const std::vector<P> &m_points;
    RangeTree<typename P::ElementType, P::Dimension> m_tree;
    Stopwatch stopwatch;

public:
    RangeQuery(const std::vector<P> &mPoints)
            : m_points(mPoints),
              m_tree(RangeTree<typename P::ElementType, P::Dimension>(mPoints)),
              stopwatch(Stopwatch()) {}

    std::vector<P> trivial(const P &from, const P &to) const {
        std::vector<P> points{};

        for (const auto item: m_points) {
            if (item >= from && item <= to) {
                points.push_back(item);
            }
        }

        std::sort(points.begin(), points.end());
        return points;
    }

    std::vector<P> efficient(const P from, const P to) const {
        return m_tree.query(from, to);
    }

    std::pair<double, double> performance(const P from, const P to) {
        stopwatch.start();
        trivial(from, to);
        stopwatch.stop();

        const double elapsedTimeTrivial = stopwatch.getElapsedTimeSeconds();

        stopwatch.reset();

        stopwatch.start();
        efficient(from, to);
        stopwatch.stop();

        const double elapsedTimeEfficient = stopwatch.getElapsedTimeSeconds();

        stopwatch.reset();

        return std::make_pair(elapsedTimeTrivial, elapsedTimeEfficient);
    }
};