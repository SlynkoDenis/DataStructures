//
// Created by denis on 21.03.2020.
//

#ifndef EULERPATH_GRAPH_H
#define EULERPATH_GRAPH_H

#endif //EULERPATH_GRAPH_H

#pragma once

#include <algorithm>
#include <cstdint>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

struct Node final {
    size_t number;
    size_t weight;

    Node() : number(0), weight(0) {};
    explicit Node(const size_t& new_number_) : number(new_number_), weight(1) {};
    Node(const size_t& new_number_, const size_t& new_weight_) : number(new_number_), weight(new_weight_) {};
    explicit operator const size_t&() const { return number; };
};

bool operator >=(const Node& lhs, const size_t& rhs) {
    return lhs.number >= rhs;
}

bool operator ==(const size_t& lhs, const Node& rhs) {
    return lhs == rhs.number;
}

bool operator ==(const Node& lhs, const size_t& rhs) {
    return lhs.number == rhs;
}

bool operator ==(const Node& lhs, const Node& rhs) {
    return lhs.number == rhs.number && lhs.weight == rhs.weight;
}

std::ostream& operator <<(std::ostream& os, const Node& node) {
    os << "(" << node.number << ", " << node.weight << ")";

    return os;
}

template <typename N>
struct Graph {
    size_t edges;
    std::vector<std::vector<N>> gList;

    Graph() : edges(0) {};
    explicit Graph(const size_t& sz) : gList(sz), edges(0) {};
    Graph(const Graph<N>& other) = default;
    Graph(Graph<N>&& other) noexcept : gList(std::move(other.gList)), edges(other.edges) {
        other.edges = 0;
    };
    virtual Graph& operator =(const Graph& other) = default;
    virtual Graph& operator =(Graph&& other) noexcept {
        gList = std::move(other.gList);
        edges = other.edges;

        other.edges = 0;

        return *this;
    };

    virtual void reserve(const size_t& cap) = 0;
    [[nodiscard]] size_t number_of_verteces() const noexcept { return gList.size(); };
    [[nodiscard]] size_t number_of_edges() const noexcept { return edges; };
    [[nodiscard]] bool empty() const noexcept { return gList.empty(); };

    virtual void addNode(size_t number, N *begin, size_t sz) = 0;

    virtual void remove_edge(size_t first, const N& last) = 0;

    virtual void remove_edge(size_t first) = 0;

    auto begin() {
        return gList.begin();
    }
    auto end() {
        return gList.end();
    }
    [[nodiscard]] const auto begin() const {
        return gList.begin();
    }
    [[nodiscard]] const auto end() const {
        return gList.end();
    }
    std::vector<N>& operator [](const size_t& index) & {
        return gList[index];
    }
    const std::vector<N>& operator [](const size_t& index) const & {
        return gList[index];
    }
    std::vector<N>&& operator [](const size_t& index) && {
        return std::move(gList[index]);
    }

    virtual ~Graph() noexcept = default;
};

template <typename N>
struct DirectedGraph final : public Graph<N> {
    std::vector<size_t> inDegrees;

    explicit DirectedGraph(size_t sz) : Graph<N>(sz), inDegrees(sz) {};
    DirectedGraph& operator =(const Graph<N>& other) override {
        if (this == &other)
            return *this;

        Graph<N>::operator=(other);
        const auto& ref = static_cast<const DirectedGraph<N>&>(other);
        inDegrees = ref.inDegrees;

        return *this;
    };
    DirectedGraph& operator =(Graph<N>&& other) noexcept override {
        if (this == &other)
            return *this;

        Graph<N>::operator=(other);
        auto&& ref = static_cast<DirectedGraph<N>&&>(other);
        inDegrees = std::move(ref.inDegrees);

        return *this;
    };
    void reserve(const size_t& cap) override {
        Graph<N>::gList.reserve(cap);
        inDegrees.reserve(cap);
    };
    void addNode(size_t number, N *begin, size_t sz) override;
    template <typename INIt> void addNode(size_t number, INIt begin, INIt end);
    void remove_edge(size_t first, const N& last) override;
    void remove_edge(size_t first) override;
    ~DirectedGraph() override = default;
};

template <typename N>
template <typename INIt> void DirectedGraph<N>::addNode(size_t number, INIt begin, INIt end) {
    auto sz = std::distance(begin, end);

    if (sz < 1)
        throw std::invalid_argument("range");
    if (sz == 1)
        return;

    if (number < Graph<N>::number_of_verteces()) {
        inDegrees[number] += sz;

        auto& iter = Graph<N>::gList[number];
        for (auto& it = begin; it != end; it = std::next(it)) {
            ++inDegrees[static_cast<size_t>(*it)];
            iter.push_back(*it);
        }

        Graph<N>::edges += sz;
    } else if (number == Graph<N>::number_of_verteces()) {
        std::vector<N> tmp;
        tmp.reserve(sz);

        for (auto& it = begin; it != end; it = std::next(it)) {
            ++inDegrees[static_cast<size_t>(*it)];
            tmp.push_back(*it);
        }

        inDegrees.push_back(sz);
        Graph<N>::gList.push_back(std::move(tmp));
        Graph<N>::edges += sz;
    } else
        throw std::invalid_argument("vertex to add");
}

template <typename N>
void DirectedGraph<N>::addNode(size_t number, N *begin, size_t sz) {
    if (!begin)
        throw std::invalid_argument("list of nodes");

    if (number < Graph<N>::number_of_verteces()) {
        inDegrees[number] += sz;

        auto& iter = Graph<N>::gList[number];
        for (size_t i = 0; i < sz; ++i) {
            ++inDegrees[static_cast<size_t>(*(begin + i))];
            iter.push_back(*(begin + i));
        }

        Graph<N>::edges += sz;
    } else if (number == Graph<N>::number_of_verteces()) {
        std::vector<N> tmp;
        tmp.reserve(sz);

        for (size_t i = 0; i < sz; ++i) {
            ++inDegrees[static_cast<size_t>(*(begin + i))];
            tmp.push_back(*(begin + i));
        }

        inDegrees.push_back(sz);
        Graph<N>::gList.push_back(std::move(tmp));
        Graph<N>::edges += sz;
    } else
        throw std::invalid_argument("vertex to add");
}

template <typename N>
void DirectedGraph<N>::remove_edge(size_t first, const N& last) {
    if (first >= Graph<N>::number_of_verteces() || last >= Graph<N>::number_of_verteces())      // N type must implement comparison with size_t type
        throw std::invalid_argument("invalid vertices");

    auto& list = this->gList[first];
    auto node = std::find(list.begin(), list.end(), last);
    if (node != list.end()) {
        list.erase(node);
        --inDegrees[static_cast<size_t>(last)];
        --Graph<N>::edges;
    }
}

template <typename N>
void DirectedGraph<N>::remove_edge(size_t first) {
    if (first >= Graph<N>::number_of_verteces())
        throw std::invalid_argument("invalid vertex");

    for (const auto& it : Graph<N>::gList[first]) {
        --inDegrees[static_cast<size_t>(it)];
    }

    auto& list = Graph<N>::gList[first];
    Graph<N>::edges -= list.size();
    list.clear();
}

template <typename N>
struct UndirectedGraph final : public Graph<N> {
    explicit UndirectedGraph(const size_t& sz) : Graph<N>(sz) {};
    UndirectedGraph(UndirectedGraph&& other) noexcept : Graph<N>(std::move(other)) {};
    UndirectedGraph& operator =(Graph<N>&& other) noexcept override {
        Graph<N>::operator=(std::move(other));

        return *this;
    }
    void reserve(const size_t& cap) override { Graph<N>::gList.reserve(cap); };
    template <typename INIt> void addNode(size_t number, INIt begin, INIt end);
    void addNode(size_t number, N *begin, size_t sz) override;
    void remove_edge(size_t first, const N& last) override;
    void remove_edge(size_t first) override;
    ~UndirectedGraph() override = default;
};

template <typename N>
template <typename INIt> void UndirectedGraph<N>::addNode(size_t number, INIt begin, INIt end) {
    auto sz = std::distance(begin, end);

    if (sz < 1)
        throw std::invalid_argument("range");
    if (sz == 1)
        return;

    if (number < Graph<N>::number_of_verteces()) {
        auto& iter = Graph<N>::gList[number];
        for (auto& it = begin; it != end; it = std::next(it)) {
            iter.push_back(*it);
            Graph<N>::gList[static_cast<size_t>(*it)].push_back(static_cast<N>(number));
        }

        Graph<N>::edges += sz;
    } else if (number == Graph<N>::number_of_verteces()) {
        std::vector<N> tmp;
        tmp.reserve(sz);

        for (auto& it = begin; it != end; it = std::next(it)) {
            tmp.push_back(*it);
            Graph<N>::gList[static_cast<size_t>(*it)].push_back(static_cast<N>(number));
        }

        Graph<N>::gList.emplace_back(std::move(tmp));
        Graph<N>::edges += sz;
    } else
        throw std::invalid_argument("vertex to add");
}

template <typename N>
void UndirectedGraph<N>::addNode(size_t number, N *begin, size_t sz) {
    if (!begin)
        throw std::invalid_argument("list of nodes");

    if (number < Graph<N>::number_of_verteces()) {
        auto& iter = Graph<N>::gList[number];
        for (size_t i = 0; i < sz; ++i) {
            iter.push_back(*(begin + i));
            Graph<N>::gList[static_cast<size_t>(*(begin + i))].push_back(static_cast<N>(number));
        }

        Graph<N>::edges += sz;
    } else if (number == Graph<N>::number_of_verteces()) {
        std::vector<N> tmp;
        tmp.reserve(sz);

        for (size_t i = 0; i < sz; ++i) {
            tmp.push_back(*(begin + i));
            Graph<N>::gList[static_cast<size_t>(*(begin + i))].push_back(static_cast<N>(number));
        }

        Graph<N>::gList.emplace_back(std::move(tmp));
        Graph<N>::edges += sz;
    } else
        throw std::invalid_argument("vertex to add");
}

template <typename N>
void UndirectedGraph<N>::remove_edge(size_t first, const N& last) {
    if (first >= Graph<N>::number_of_verteces() || last >= Graph<N>::number_of_verteces())      // N type must implement comparison with size_t type
        throw std::invalid_argument("invalid vertices");

    auto& list = Graph<N>::gList[first];
    auto node = std::find(list.begin(), list.end(), last);

    if (node != list.end())
        list.erase(node);

    auto& last_list = Graph<N>::gList[static_cast<size_t>(last)];
    node = std::find_if(last_list.begin(), last_list.end(), [first](const N& other) {
        return first == other;
    });
    if (node != last_list.end()) {
        --this->edges;
        last_list.erase(node);
    }
}

template <typename N>
void UndirectedGraph<N>::remove_edge(size_t first) {
    if (first >= Graph<N>::number_of_verteces())
        throw std::invalid_argument("invalid vertex");

    auto& list = Graph<N>::gList[first];
    for (size_t i = 0, end_ = list.size(); i < end_; ++i) {
        auto& tmp = Graph<N>::gList[static_cast<size_t>(list[i])];
        auto iter = std::find_if(tmp.begin(), tmp.end(), [first](const N& other) {
            return first == other;
        });
        tmp.erase(iter);
    }

    Graph<N>::edges -= list.size();
    list.clear();
}

namespace {
    size_t generate_optimal_number(const size_t &number_of_vertices, const size_t &number_of_edges) {
        size_t tmp = number_of_edges / number_of_vertices;
        if (tmp + 2 < number_of_vertices)
            return tmp + 2;
        else
            return number_of_vertices / 2;
    }
}

template <typename T>
std::ostream& operator <<(std::ostream& os, const std::vector<T>& v) {
    for (const auto& it : v)
        os << it << " ";
    return os;
}

template <typename N>
std::ostream& operator <<(std::ostream& os, const Graph<N>& g) {
    for (size_t i = 0, end_ = g.number_of_verteces() - 1; i < end_; ++i) {
        os << i << ": " << g[i] << std::endl;
    }
    os << g.number_of_verteces() - 1 << ": " << g[g.number_of_verteces() - 1];

    return os;
}

DirectedGraph<Node> generate_directed_graph(const size_t& number_of_vertices, const size_t& number_of_edges, const size_t& max_weight) {
    if (static_cast<uint64_t>(number_of_edges) > static_cast<uint64_t>(number_of_vertices) * (number_of_vertices - 1))
        throw std::invalid_argument("cannot create graph with such number of edges");

    DirectedGraph<Node> result(number_of_vertices);
    result.edges = number_of_edges;

    size_t already_generated = 0;
    size_t opt_number = generate_optimal_number(number_of_vertices, number_of_edges);

    std::random_device rd0;
    std::mt19937 gen0(rd0());
    std::uniform_int_distribution<> weight_dis(1, max_weight);

    std::random_device rd1;
    std::mt19937 gen1(rd1());
    std::uniform_int_distribution<> vert_dis(0, number_of_vertices - 1);

    std::random_device rd2;
    std::mt19937 gen2(rd2());
    std::uniform_int_distribution<> edge_dis(1, std::min(opt_number, number_of_vertices - 1));

    std::vector<bool> vertices(number_of_vertices, false);

    std::vector<bool> tmp(number_of_vertices, false);
    while (already_generated != number_of_edges) {
        size_t current_index = vert_dis(gen1);
        while (vertices[current_index]) {
            current_index = vert_dis(gen1);
        }
        vertices[current_index] = true;

        size_t current_edges = edge_dis(gen2);
        while (already_generated > number_of_edges - current_edges) {
            current_edges = edge_dis(gen2);
        }
        already_generated += current_edges;

        size_t tmp_counter = 0;
        std::fill(tmp.begin(), tmp.end(),false);
        tmp[current_index] = true;

        std::vector<Node> tmp_res;
        tmp_res.reserve(current_edges);
        while (tmp_counter != current_edges) {
            size_t tmp_index = current_index;
            while (tmp[tmp_index]) {
                tmp_index = vert_dis(gen1);
            }

            ++tmp_counter;
            tmp[tmp_index] = true;

            ++result.inDegrees[tmp_index];
            tmp_res.emplace_back(tmp_index, weight_dis(gen0));
        }

        result.gList[current_edges] = std::move(tmp_res);
    }

    return result;
}

DirectedGraph<size_t> generate_directed_graph(const size_t& number_of_vertices, const size_t& number_of_edges) {
    if (static_cast<uint64_t>(number_of_edges) > static_cast<uint64_t>(number_of_vertices) * (number_of_vertices - 1))
        throw std::invalid_argument("cannot create graph with such number of edges");

    DirectedGraph<size_t> result(number_of_vertices);

    size_t already_generated = 0;
    size_t opt_number = generate_optimal_number(number_of_vertices, number_of_edges);

    std::random_device rd1;
    std::mt19937 gen1(rd1());
    std::uniform_int_distribution<> vert_dis(0, number_of_vertices - 1);

    std::random_device rd2;
    std::mt19937 gen2(rd2());
    std::uniform_int_distribution<> edge_dis(1, std::min(opt_number, number_of_vertices - 1));

    std::vector<bool> vertices(number_of_vertices, false);

    std::vector<bool> tmp(number_of_vertices, false);
    while (already_generated != number_of_edges) {
        size_t current_index = vert_dis(gen1);
        size_t current_edges = edge_dis(gen2);

        if (std::count(vertices.begin(), vertices.end(), false) > 1) {
            while (vertices[current_index]) {
                current_index = vert_dis(gen1);
            }

            while (already_generated + current_edges > number_of_edges) {
                current_edges = edge_dis(gen2);
            }
            already_generated += current_edges;
        } else {
            current_index = std::find(vertices.begin(), vertices.end(), false) - vertices.begin();

            current_edges = number_of_edges - already_generated;
            already_generated = number_of_edges;
        }

        vertices[current_index] = true;

        size_t tmp_counter = 0;
        std::fill(tmp.begin(), tmp.end(), false);
        tmp[current_index] = true;

        std::vector<size_t> tmp_res;
        tmp_res.reserve(current_edges);
        while (tmp_counter != current_edges) {
            size_t tmp_index = current_index;
            while (tmp[tmp_index]) {
                tmp_index = vert_dis(gen1);
            }

            ++tmp_counter;
            tmp[tmp_index] = true;

            ++result.inDegrees[tmp_index];
            tmp_res.emplace_back(tmp_index);
        }

        result.gList[current_index] = std::move(tmp_res);
    }

    return result;
}

UndirectedGraph<size_t> generate_undirected_graph(const size_t& number_of_vertices, const size_t& number_of_edges) {
    if (static_cast<uint64_t>(number_of_edges) > static_cast<uint64_t>(number_of_vertices) * (number_of_vertices - 1) / 2)
        throw std::invalid_argument("cannot create graph with such number of edges");

    UndirectedGraph<size_t> result(number_of_vertices);

    size_t already_generated = 0;
    size_t opt_number = generate_optimal_number(number_of_vertices, number_of_edges);

    std::random_device rd1;
    std::mt19937 gen1(rd1());
    std::uniform_int_distribution<> vert_dis(0, number_of_vertices - 1);

    std::random_device rd2;
    std::mt19937 gen2(rd2());
    std::uniform_int_distribution<> edge_dis(1, std::min(opt_number, number_of_vertices - 1));

    std::vector<bool> vertices(number_of_vertices, false);

    std::vector<bool> tmp(number_of_vertices, false);
    while (already_generated != number_of_edges) {
        size_t current_index = vert_dis(gen1);
        size_t current_edges = edge_dis(gen2);

        if (std::count(vertices.begin(), vertices.end(), false) > 1) {
            while (vertices[current_index]) {
                current_index = vert_dis(gen1);
            }

            while (already_generated + current_edges > number_of_edges) {
                current_edges = edge_dis(gen2);
            }
            already_generated += current_edges;
        } else {
            current_index = std::find(vertices.begin(), vertices.end(), false) - vertices.begin();

            current_edges = number_of_edges - already_generated;
            already_generated = number_of_edges;

            while (current_edges >= number_of_vertices) {
                std::vector<bool> tmp_mask(number_of_vertices, false);

                for (size_t i = 0; i < number_of_vertices; ++i) {
                    if (result.gList[i].size() < number_of_vertices) {
                        size_t decrement = number_of_vertices - result.gList[i].size();

                        std::fill(tmp_mask.begin(), tmp_mask.end(), false);
                        tmp_mask[i] = true;
                        for (auto& it : result.gList[i])
                            tmp_mask[it] = true;

                        size_t tmp_counter = 0;
                        while (tmp_counter != decrement) {
                            size_t tmp_index = i;
                            while (tmp_mask[i])
                                tmp_index = vert_dis(gen1);

                            ++tmp_counter;
                            tmp_mask[tmp_index] = true;

                            result.gList[tmp_index].emplace_back(i);
                            result.gList[i].emplace_back(tmp_index);
                        }

                        current_edges -= decrement;
                    }
                }
            }
        }

        vertices[current_index] = true;

        size_t initial_size = result.gList[current_index].size();

        size_t tmp_counter = 0;
        std::fill(tmp.begin(), tmp.end(), false);
        tmp[current_index] = true;

        if (initial_size) {
            for (const auto& it : result.gList[current_index])
                tmp[it] = true;
        }

        std::vector<size_t> tmp_res;
        tmp_res.reserve(current_edges);
        while (tmp_counter != current_edges) {
            size_t tmp_index = current_index;
            while (tmp[tmp_index]) {
                tmp_index = vert_dis(gen1);
            }

            ++tmp_counter;
            tmp[tmp_index] = true;

            tmp_res.emplace_back(tmp_index);
            result.gList[tmp_index].push_back(current_index);
        }

        result.gList[current_index].insert(result.gList[current_index].end(), std::make_move_iterator(tmp_res.begin()), std::make_move_iterator(tmp_res.end()));
    }

    return result;
}
