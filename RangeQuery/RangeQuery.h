//
// Created by Yannick Huggler on 20.01.22.
//

#include "RangeTree.hpp"

template<class P>
class RangeQuery {
    const std::vector<P> &m_points;
    RangeTree<typename P::ElementType, P::Dimension> m_tree;

public:
    RangeQuery(const std::vector<P> &mPoints) : m_points(mPoints),
                                                m_tree(RangeTree<typename P::ElementType, P::Dimension>(mPoints)) {}

    std::vector<P> trivial(const P &from, const P &to) const {
        std::vector<P> points{};

        for (const auto &item: m_points) {
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
};