//
// Created by denis on 22.03.2020.
//
#pragma once

#ifndef DISJOINTSETUNION_DSU_H
#define DISJOINTSETUNION_DSU_H

#endif //DISJOINTSETUNION_DSU_H

#include <stdexcept>
#include <vector>

template <typename N>
struct Node {
    size_t rank;
    N data;
    Node *parent;

    explicit Node(const N& new_data_) : data(new_data_), rank(0), parent(nullptr) {};
    explicit Node(N&& new_data_) : data(std::move(new_data_)), rank(0), parent(nullptr) {};
    Node *root();
};

template <typename N>
Node<N> *Node<N>::root() {
    Node<N> *result = parent;
    if (!result)
        return this;

    while (result->parent)
        result = result->parent;

    return result;
}

template <typename N>
class DSU final {
    std::vector<Node<N> *> universum;

public:
    explicit DSU(size_t sz) : universum(sz, nullptr) {};
    void reserve(size_t cap) {
        universum.reserve(cap);
    };
    Node<N> *add_element(const N& data);
    Node<N> *add_element(N&& data);
    void unite(Node<N> *lhs, Node<N> *rhs);
    bool equivalent(Node<N> *lhs, Node<N> *rhs) const;
};

template <typename N>
Node<N> *DSU<N>::add_element(const N &data) {
    universum.push_back(new Node<N>(data));
    return universum.back();
}

template <typename N>
Node<N> *DSU<N>::add_element(N&& data) {
    universum.push_back(new Node<N>(std::move(data)));
    return universum.back();
}

template <typename N>
bool DSU<N>::equivalent(Node<N> *lhs, Node<N> *rhs) const {
    if (lhs == nullptr || rhs == nullptr)
        throw std::invalid_argument("elements");

    auto lparent = lhs->root();
    auto rparent = rhs->root();

    for (auto it = lhs; it != lparent; it = it->parent) {
        it->parent = lparent;
    }
    for (auto it = rhs; it != rparent; it = it->parent) {
        it->parent = rparent;
    }

    return (lparent == rhs->root());
}

template <typename N>
void DSU<N>::unite(Node<N> *lhs, Node<N> *rhs) {
    if (lhs == nullptr || rhs == nullptr)
        throw std::invalid_argument("elements");

    auto lparent = lhs->root();
    auto rparent = rhs->root();

    for (auto it = lhs; it != lparent; it = it->parent) {
        it->parent = lparent;
    }
    for (auto it = rhs; it != rparent; it = it->parent) {
        it->parent = rparent;
    }

    if (lparent == rparent)
        return;
    else {
        if (lparent->rank < rparent->rank) {
            lparent->parent = rparent;
        } else {
            rparent->parent = lparent;
            if (lparent->rank == rparent->rank)
                ++lparent->rank;
        }
    }
}