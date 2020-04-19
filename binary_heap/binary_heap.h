//
// Created by denis on 20.03.2020.
//

#pragma once

#ifndef BINARY_HEAP_BINARY_HEAP_H
#define BINARY_HEAP_BINARY_HEAP_H

#endif //BINARY_HEAP_BINARY_HEAP_H

#include <stdexcept>
#include <vector>

template <typename K, typename V>
class Node final {
    K key;
    V value;

public:
    Node() : key(), value() {};
    Node(const K& new_key, const V& new_value) : key(new_key), value(new_value) {};
    Node(K&& new_key, V&& new_value) : key(std::move(new_key)), value(std::move(new_value)) {};
    const K& get_key() const {
        return key;
    };
    V& get_value() {
        return value;
    };
    const V& get_value() const {
        return value;
    };
    void set_key(const K& new_key) {
        key = new_key;
    };
    void swap(Node& other) {
        std::swap(key, other.key);
        std::swap(value, other.value);
    }
};

using BinaryHeapIterator = size_t;

template <typename K, typename V>
class BinaryHeap final {
    std::vector<Node<K, V>> nodes;

    BinaryHeapIterator sift_up(BinaryHeapIterator iter);
    BinaryHeapIterator sift_down(BinaryHeapIterator iter);
    BinaryHeapIterator get_min_iterator(const BinaryHeapIterator& lhs, const BinaryHeapIterator& rhs) {
        if (lhs >= nodes.size() && rhs >= nodes.size())
            return nodes.size();
        if (lhs >= nodes.size())
            return rhs;
        if (rhs >= nodes.size())
            return lhs;

        return nodes[lhs].get_key() <= nodes[rhs].get_key() ? lhs : rhs;
    }

public:
    BinaryHeap() = default;
    template <typename InputIt>
    BinaryHeap(InputIt first, InputIt last);
    void reserve(size_t number) {
        nodes.reserve(number);
    };
    [[nodiscard]] size_t size() const { return nodes.size(); };
    [[nodiscard]] bool empty() const { return nodes.empty(); };
    BinaryHeapIterator insert(const K& key, const V& value);
    Node<K, V> extract_min();
    Node<K, V> get_min() const;
    BinaryHeapIterator decrease_key(BinaryHeapIterator iter, K new_key);
    Node<K, V> delete_element(BinaryHeapIterator iter);
};

template <typename K, typename V>
template <typename InputIt> BinaryHeap<K, V>::BinaryHeap(InputIt first, InputIt last) : nodes(first, last) {
    int mid = nodes.size() / 2;

    for (int i = mid; i >= 0; --i) {
        auto tmp = get_min_iterator(2 * i + 1, 2 * i + 2);

        if (tmp == nodes.size())
            continue;

        if (nodes[i].get_key() > nodes[tmp].get_key()) {
            sift_down(i);
        }
    }
}

template <typename K, typename V>
BinaryHeapIterator BinaryHeap<K, V>::sift_up(BinaryHeapIterator iter) {
    if (iter >= nodes.size())
        throw std::logic_error("sift_up overflow");
    if (iter == 0)
        return iter;

    K position_key = nodes[iter].get_key();
    while (iter >= 0 && nodes[(iter - 1) / 2].get_key() > position_key) {
        nodes[iter].swap(nodes[(iter - 1) / 2]);
        iter = (iter - 1) / 2;
    }

    return iter;
}

template <typename K, typename V>
BinaryHeapIterator BinaryHeap<K, V>::sift_down(BinaryHeapIterator iter) {
    if (iter >= nodes.size())
        throw std::logic_error("sift_down overflow");

    K position_key = nodes[iter].get_key();
    BinaryHeapIterator tmp_iter = get_min_iterator(2 * iter + 1, 2 * iter + 2);
    BinaryHeapIterator end_ = nodes.size();
    while (tmp_iter != end_ && position_key > nodes[tmp_iter].get_key()) {
        nodes[iter].swap(nodes[tmp_iter]);
        iter = tmp_iter;
        tmp_iter = get_min_iterator(2 * iter + 1, 2 * iter + 2);
    }

    return iter;
}

template <typename K, typename V>
BinaryHeapIterator BinaryHeap<K, V>::insert(const K& key, const V& value) {
    nodes.push_back(Node(key, value));

    return sift_up(nodes.size() - 1);
}

template <typename K, typename V>
Node<K, V> BinaryHeap<K, V>::get_min() const {
    if (!nodes.empty())
        return nodes[0];
    else
        throw std::logic_error("get_min underflow");
}

template <typename K, typename V>
Node<K, V> BinaryHeap<K, V>::extract_min() {
    if (nodes.empty())
        throw std::logic_error("extract_min underflow");
    if (nodes.size() == 1) {
        auto result = nodes[0];
        nodes.pop_back();
        return result;
    }

    auto result = nodes[0];
    nodes[0].swap(nodes[nodes.size() - 1]);
    nodes.pop_back();
    sift_down(0);

    return result;
}

template <typename K, typename V>
Node<K, V> BinaryHeap<K, V>::delete_element(BinaryHeapIterator iter) {
    if (iter >= nodes.size())
        throw std::logic_error("decrease_key overflow");

    auto result = nodes[iter];

    nodes[nodes.size() - 1].swap(nodes[iter]);
    nodes.pop_back();
    sift_down(iter);

    return result;
}

template <typename K, typename V>
BinaryHeapIterator BinaryHeap<K, V>::decrease_key(BinaryHeapIterator iter, K new_key) {
    if (iter >= nodes.size())
        throw std::logic_error("decrease_key overflow");
    if (new_key > nodes[iter])
        throw std::logic_error("new key in decrease_key exceeds the existing key");

    nodes[iter].set_key(new_key);
    return sift_up(iter);
}

