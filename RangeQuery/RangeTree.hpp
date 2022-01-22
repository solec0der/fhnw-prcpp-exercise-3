#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include "Point.h"

///////////////////////////////////////////////////////////////////////////////
// RangeTree for range queries.
// Duplicates are correctly handled.
// Author: C. Stamm
// Co-Author: Yannick Huggler
// 

///////////////////////////////////////////////////////////////////////////////
template<typename T, dim_t D> using SpVec = std::vector<std::shared_ptr<Point<T, D>>>;
template<typename T, dim_t D> using SpIt = typename SpVec<T, D>::iterator;

template<typename T, dim_t D>
static void sortPoints(const SpIt<T, D> &beg, const SpIt<T, D> &end, dim_t coord) {
    sort(beg, end, [coord](const std::shared_ptr<Point<T, D>> &a, const std::shared_ptr<Point<T, D>> &b) {
        return (*a)[coord] < (*b)[coord];
    });
}


template<typename T, dim_t L>
class Node {
    using AssocUP = std::unique_ptr<Node<T, L - 1>>;
    using AssocPtr = Node<T, L - 1> *;

    AssocUP m_assoc;

public:
    explicit Node(AssocUP&& assoc) : m_assoc(std::move(assoc)) {}

    virtual ~Node() = default;

    Node<T, L - 1> *assoc() const {
        return m_assoc.get();
    }

    virtual const T &key() const = 0;

    virtual void print(std::ostream &os) const = 0;
};

template<typename T>
class Node<T, 1> {

public:
    virtual ~Node() = default;

    virtual const T &key() const = 0;

    virtual void print(std::ostream &os) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// TODO implement abstract base class Node for point element type T
// and for L > 1. A tree of D-dim points with L > 1 is a search tree of
// the (1 + D - L)-th coordinates of the D-dim points.
// Example: D = 3, L = 3: it is a search tree for the x-coordinates of all stored 
// 3-dim points p(x,y,z).

///////////////////////////////////////////////////////////////////////////////
// TODO implement specialized abstract base class Node for point element type T
// and for L = 1. A tree of D-dim points with L = 1 is a search tree for the last
// coordinates of the D-dim points.
// Example: D = 3, L = 1: it is a search tree for the z-coordinates of all stored 
// 3-dim points p(x,y,z).

///////////////////////////////////////////////////////////////////////////////
// general classes

///////////////////////////////////////////////////////////////////////////////
template<typename T, dim_t L>
class InnerNode : public Node<T, L> {
    using AssocUP = std::unique_ptr<Node<T, L - 1>>;
    using NodeUP = std::unique_ptr<Node<T, L>>;
    using NodePtr = const Node<T, L> *;

    NodeUP m_left, m_right;
    T m_key;

public:
    InnerNode(const T &key, NodeUP &&left, NodeUP &&right, AssocUP &&assoc)
            : Node<T, L>(std::move(assoc)), m_left(std::move(left)), m_right(std::move(right)), m_key(key) {}

    NodePtr left() const { return m_left.get(); }

    NodePtr right() const { return m_right.get(); }

    const T &key() const override { return m_key; }

    void print(std::ostream &os) const override {
        m_left->print(os);
        os << ",{";
        this->assoc()->print(os);
        os << "},";
        m_right->print(os);
    }
};

///////////////////////////////////////////////////////////////////////////////
template<typename T, dim_t L, dim_t D>
class LeafNode : public Node<T, L> {
    using AssocUP = std::unique_ptr<Node<T, L - 1>>;

    std::shared_ptr<Point<T, D>> m_point;

public:
    LeafNode(const std::shared_ptr<Point<T, D>> &point, AssocUP &&assoc)
            : Node<T, L>(std::move(assoc)), m_point(point) {}

    Point<T, D> get() const { return *m_point; }

    const T &key() const override { return (*m_point)[D - L]; }

    void print(std::ostream &os) const override { os << *m_point; }
};

///////////////////////////////////////////////////////////////////////////////
template<typename T, dim_t L, dim_t D = L>
class RangeTree {
    using AssocUP = std::unique_ptr<Node<T, L - 1>>;
    using NodeUP = std::unique_ptr<Node<T, L>>;
    using NodePtr = const Node<T, L> *;
    using LeafPtr = const LeafNode<T, L, D> *;
    using InnerPtr = const InnerNode<T, L> *;

    NodeUP m_root;
    size_t m_size;

public:
    RangeTree(std::vector<Point<T, D>> points) : m_size(points.size()) {
        SpVec<T, D> spoints(m_size);
        auto it = spoints.begin();

        for (const Point<T, D> &p: points) *it++ = std::make_shared<Point<T, D>>(p);
        ::sortPoints<T, D>(spoints.begin(), it, D - L);
        m_root = buildTree(spoints.begin(), it);
    }

    static NodeUP buildTree(const SpIt<T, D> &beg, const SpIt<T, D> &end) {
        if (end - beg == 1) {
            return std::make_unique<LeafNode<T, L, D>>(*beg, std::move(RangeTree<T, L - 1, D>::buildTree(beg, end)));
        } else {
            SpIt<T, D> m = beg + (end - beg) / 2;
            const T key = (**(m - 1))[D -
                                      L];    // must be called before buildAssocTree, because it changes order of points
            auto left = buildTree(beg,
                                  m);        // must be called before buildAssocTree, because it changes order of points
            auto right = buildTree(m,
                                   end);        // must be called before buildAssocTree, because it changes order of points
            return std::make_unique<InnerNode<T, L>>(key, std::move(left), std::move(right), buildAssocTree(beg, end));
        }
    }

    static AssocUP buildAssocTree(const SpIt<T, D> &beg, const SpIt<T, D> &end) {
        ::sortPoints<T, D>(beg, end, D - L + 1);
        return RangeTree<T, L - 1, D>::buildTree(beg, end);
    }

    std::vector<Point<T, D>> query(const Point<T, D> &from, const Point<T, D> &to) const {
        std::vector<Point<T, D>> result;

        query(m_root.get(), from, to.nextAfter(), result);
        return result;
    }

    static void query(NodePtr v, const Point<T, D> &from, const Point<T, D> &to, std::vector<Point<T, D>> &result) {
        const T &fromKey = from[D - L];
        const T &toKey = to[D - L];

        v = findSplitNode(v, fromKey, toKey);
        auto lv = dynamic_cast<LeafPtr>(v);

        if (lv) {
            // v is a leaf
            if (fromKey <= lv->key() && lv->key() < toKey) {
                RangeTree<T, L - 1, D>::query(lv->assoc(), from, to, result);
            }

        } else {
            // vsplit is an innerNode
            auto ivs = static_cast<InnerPtr>(v);

            // follow the path to 'from' and report the points in subtrees right of the path
            v = ivs->left();
            lv = dynamic_cast<LeafPtr>(v);

            while (!lv) {
                auto iv = static_cast<InnerPtr>(v);

                if (fromKey <= iv->key()) {
                    RangeTree<T, L - 1, D>::query(iv->right()->assoc(), from, to, result);
                    v = iv->left();
                } else {
                    v = iv->right();
                }
                lv = dynamic_cast<LeafPtr>(v);
            }
            if (fromKey <= lv->key() && lv->key() < toKey) {
                RangeTree<T, L - 1, D>::query(lv->assoc(), from, to, result);
            }

            // follow the path to 'to' and report the points in subtrees left of the path
            v = ivs->right();
            lv = dynamic_cast<LeafPtr>(v);

            while (!lv) {
                auto iv = static_cast<InnerPtr>(v);

                if (iv->key() < toKey) {
                    RangeTree<T, L - 1, D>::query(iv->left()->assoc(), from, to, result);
                    v = iv->right();
                } else {
                    v = iv->left();
                }
                lv = dynamic_cast<LeafPtr>(v);
            }
            if (fromKey <= lv->key() && lv->key() < toKey) {
                RangeTree<T, L - 1, D>::query(lv->assoc(), from, to, result);
            }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const RangeTree<T, L, D> &rt) {
        os << '[';
        rt.m_root->print(os);
        return os << ']';
    }

private:
    static NodePtr findSplitNode(NodePtr v, const T &from, const T &to) {
        auto *lv = dynamic_cast<LeafPtr>(v);

        while (!lv && (to <= v->key() || v->key() < from)) {
            auto *iv = static_cast<InnerPtr>(v);

            if (to <= v->key()) {
                v = iv->left();
            } else {
                v = iv->right();
            }
            lv = dynamic_cast<LeafPtr>(v);
        }
        return v;
    }
};


///////////////////////////////////////////////////////////////////////////////
// specialization
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
template<typename T>
class InnerNode<T, 1> : public Node<T, 1> {
    using NodeUP = std::unique_ptr<Node<T, 1>>;
    using NodePtr = const Node<T, 1> *;

    NodeUP m_left, m_right;
    T m_key;

public:
    InnerNode(const T &key, NodeUP &&left, NodeUP &&right)
            : m_left(std::move(left)), m_right(std::move(right)), m_key(key) {}

    NodePtr left() const { return m_left.get(); }

    NodePtr right() const { return m_right.get(); }

    const T &key() const override { return m_key; }

    void print(std::ostream &os) const override {
        m_left->print(os);
        os << ',';
        m_right->print(os);
    }
};

///////////////////////////////////////////////////////////////////////////////
template<typename T, dim_t D>
class LeafNode<T, 1, D> : public Node<T, 1> {
    std::shared_ptr<Point<T, D>> m_point;

public:
    LeafNode(const std::shared_ptr<Point<T, D>> &point) : m_point(point) {}

    Point<T, D> get() const { return *m_point; }

    const T &key() const override { return (*m_point)[D - 1]; }

    void print(std::ostream &os) const override { os << *m_point; }
};

///////////////////////////////////////////////////////////////////////////////
template<typename T, dim_t D>
class RangeTree<T, 1, D> {
    using NodeUP = std::unique_ptr<Node<T, 1>>;
    using NodePtr = const Node<T, 1> *;
    using LeafPtr = const LeafNode<T, 1, D> *;
    using InnerPtr = const InnerNode<T, 1> *;

    NodeUP m_root;
    size_t m_size;

public:
    RangeTree(std::vector<Point<T, D>> points) : m_size(points.size()) {
        SpVec<T, D> spoints(m_size);
        auto it = spoints.begin();

        for (const Point<T, D> &p: points) *it++ = std::make_shared<Point<T, D>>(p);
        ::sortPoints<T, D>(spoints.begin(), it, D - 1);
        m_root = buildTree(spoints.begin(), it);
    }

    static NodeUP buildTree(const SpIt<T, D> &beg, const SpIt<T, D> &end) {
        if (end - beg == 1) {
            return std::make_unique<LeafNode<T, 1, D>>(*beg);
        } else {
            SpIt<T, D> m = beg + (end - beg) / 2;
            return std::make_unique<InnerNode<T, 1>>((**(m - 1))[D - 1], buildTree(beg, m), buildTree(m, end));
        }
    }

    std::vector<Point<T, D>> query(const Point<T, D> &from, const Point<T, D> &to) const {
        std::vector<Point<T, D>> result;

        query(m_root.get(), from, to.nextAfter(), result);
        return result;
    }

    static void query(NodePtr v, const Point<T, D> &from, const Point<T, D> &to, std::vector<Point<T, D>> &result) {
        const T &fromKey = from[D - 1];
        const T &toKey = to[D - 1];

        v = findSplitNode(v, fromKey, toKey);
        auto lv = dynamic_cast<LeafPtr>(v);

        if (lv) {
            // v is a leaf
            if (fromKey <= lv->key() && lv->key() < toKey) {
                result.push_back(lv->get());
            }

        } else {
            // vsplit is an innerNode
            auto ivs = static_cast<InnerPtr>(v);

            // follow the path to 'from' and report the points in subtrees right of the path
            v = ivs->left();
            lv = dynamic_cast<LeafPtr>(v);

            while (!lv) {
                auto iv = static_cast<InnerPtr>(v);

                if (fromKey <= iv->key()) {
                    reportSubtree(iv->right(), result);
                    v = iv->left();
                } else {
                    v = iv->right();
                }
                lv = dynamic_cast<LeafPtr>(v);
            }
            if (fromKey <= lv->key() && lv->key() < toKey) {
                result.push_back(lv->get());
            }

            // follow the path to 'to' and report the points in subtrees left of the path
            v = ivs->right();
            lv = dynamic_cast<LeafPtr>(v);

            while (!lv) {
                auto iv = static_cast<InnerPtr>(v);

                if (iv->key() < toKey) {
                    reportSubtree(iv->left(), result);
                    v = iv->right();
                } else {
                    v = iv->left();
                }
                lv = dynamic_cast<LeafPtr>(v);
            }
            if (fromKey <= lv->key() && lv->key() < toKey) {
                result.push_back(lv->get());
            }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const RangeTree<T, 1, D> &rt) {
        os << '[';
        rt.m_root->print(os);
        return os << ']';
    }

private:
    static NodePtr findSplitNode(NodePtr v, const T &from, const T &to) {
        auto lv = dynamic_cast<LeafPtr>(v);

        while (!lv && (to <= v->key() || v->key() < from)) {
            auto iv = static_cast<InnerPtr>(v);

            if (to <= v->key()) {
                v = iv->left();
            } else {
                v = iv->right();
            }
            lv = dynamic_cast<LeafPtr>(v);
        }
        return v;
    }

    static void reportSubtree(NodePtr v, std::vector<Point<T, D>> &result) {
        auto lv = dynamic_cast<LeafPtr>(v);

        if (lv) {
            // v is a leaf
            result.push_back(lv->get());
        } else {
            // v is an innerNode
            auto iv = static_cast<InnerPtr>(v);

            reportSubtree(iv->left(), result);
            reportSubtree(iv->right(), result);
        }
    }
};


