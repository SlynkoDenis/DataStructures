//
// Created by denis on 21.03.2020.
//

#ifndef EULERPATH_GRAPH_H
#define EULERPATH_GRAPH_H

#endif //EULERPATH_GRAPH_H

#pragma once

#include <algorithm>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

template <typename T>
std::ostream& operator <<(std::ostream& os, const std::vector<T>& v) {
    for (const auto& it : v)
        os << it << " ";
    return os;
}

struct Node {
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
    virtual Graph& operator =(const Graph& other) = default;

    virtual void reserve(const size_t& cap) = 0;
    [[nodiscard]] size_t number_of_verteces() const { return gList.size(); };
    [[nodiscard]] size_t number_of_edges() const { return edges; };
    [[nodiscard]] bool empty() const { return gList.empty(); };

    virtual void addNode(size_t number, N *begin, size_t sz) = 0;

    virtual void remove_edge(size_t first, const N& last) = 0;

    virtual void remove_edge(size_t first) = 0;

    auto begin() {
        return gList.begin();
    }
    auto end() {
        return gList.end();
    }
    [[nodiscard]] auto begin() const {
        return gList.begin();
    }
    [[nodiscard]] auto end() const {
        return gList.end();
    }
    std::vector<N>& operator [](const size_t& index) {
        return gList[index];
    }
    const std::vector<N>& operator [](const size_t& index) const {
        return gList[index];
    }

    virtual ~Graph() {};
};

template <typename N>
struct DirectedGraph final : public Graph<N> {
    std::vector<size_t> inDegrees;

    explicit DirectedGraph(size_t sz) : Graph<N>(sz), inDegrees(sz) {};
    DirectedGraph& operator =(const Graph<N>& other) override {
        const DirectedGraph<N>& ref = static_cast<const DirectedGraph<N>&>(other);
        this->gList = ref.gList;
        this->edges = ref.edges;
        inDegrees = ref.inDegrees;

        return *this;
    };
    void reserve(const size_t& cap) override {
        this->gList.reserve(cap);
        inDegrees.reserve(cap);
    };
    void addNode(size_t number, N *begin, size_t sz) override;
    void remove_edge(size_t first, const N& last) override;
    void remove_edge(size_t first) override;
    ~DirectedGraph() override = default;
};

template <typename N>
void DirectedGraph<N>::addNode(size_t number, N *begin, size_t sz) {
    if (!begin)
        throw std::invalid_argument("list of nodes");

    if (number < this->number_of_verteces()) {
        inDegrees[number] += sz;

        auto& iter = this->gList[number];
        for (size_t i = 0; i < sz; ++i) {
            ++inDegrees[static_cast<size_t>(*(begin + i))];
            iter.push_back(*(begin + i));
        }

        this->edges += sz;
    } else if (number == this->number_of_verteces()) {
        std::vector<N> tmp;
        tmp.reserve(sz);

        for (size_t i = 0; i < sz; ++i) {
            ++inDegrees[static_cast<size_t>(*(begin + i))];
            tmp.push_back(*(begin + i));
        }

        inDegrees.push_back(sz);
        this->gList.push_back(std::move(tmp));
        this->edges += sz;
    } else
        throw std::invalid_argument("vertex to add");
}

template <typename N>
void DirectedGraph<N>::remove_edge(size_t first, const N& last) {
    if (first >= this->number_of_verteces() || last >= this->number_of_verteces())      // N type must implement comparison with size_t type
        throw std::invalid_argument("invalid vertices");

    auto& list = this->gList[first];
    auto node = std::find(list.begin(), list.end(), last);
    if (node != list.end()) {
        list.erase(node);
        --inDegrees[static_cast<size_t>(last)];
        --this->edges;
    }
}

template <typename N>
void DirectedGraph<N>::remove_edge(size_t first) {
    if (first >= this->number_of_verteces())
        throw std::invalid_argument("invalid vertex");

    for (const auto& it : this->gList[first]) {
        --inDegrees[static_cast<size_t>(it)];
    }

    auto& list = this->gList[first];
    this->edges -= list.size();
    list.clear();
}

template <typename N>
struct UndirectedGraph final : public Graph<N> {
    explicit UndirectedGraph(const size_t& sz) : Graph<N>(sz) {};
    UndirectedGraph& operator =(const Graph<N>& other) override {
        const UndirectedGraph<N>& ref = static_cast<const UndirectedGraph<N>&>(other);
        this->gList = ref.gList;
        this->edges = ref.edges;

        return *this;
    };
    void reserve(const size_t& cap) override { this->gList.reserve(cap); };
    void addNode(size_t number, N *begin, size_t sz) override;
    void remove_edge(size_t first, const N& last) override;
    void remove_edge(size_t first) override;
    ~UndirectedGraph() override = default;
};

template <typename N>
void UndirectedGraph<N>::addNode(size_t number, N *begin, size_t sz) {
    if (!begin)
        throw std::invalid_argument("list of nodes");

    if (number < this->number_of_verteces()) {
        auto& iter = this->gList[number];
        for (size_t i = 0; i < sz; ++i) {
            iter.push_back(*(begin + i));
            this->gList[static_cast<size_t>(*(begin + i))].push_back(static_cast<N>(number));
        }

        this->edges += sz;
    } else if (number == this->number_of_verteces()) {
        std::vector<N> tmp;
        tmp.reserve(sz);

        for (size_t i = 0; i < sz; ++i) {
            tmp.push_back(*(begin + i));
            this->gList[static_cast<size_t>(*(begin + i))].push_back(static_cast<N>(number));
        }

        this->gList.push_back(std::move(tmp));
        this->edges += sz;
    } else
        throw std::invalid_argument("vertex to add");
}

template <typename N>
void UndirectedGraph<N>::remove_edge(size_t first, const N& last) {
    if (first >= this->number_of_verteces() || last >= this->number_of_verteces())      // N type must implement comparison with size_t type
        throw std::invalid_argument("invalid vertices");

    auto& list = this->gList[first];
    auto node = std::find(list.begin(), list.end(), last);

    if (node != list.end())
        list.erase(node);

    auto& last_list = this->gList[static_cast<size_t>(last)];
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
    if (first >= this->number_of_verteces())
        throw std::invalid_argument("invalid vertex");

    auto& list = this->gList[first];
    for (size_t i = 0, end_ = list.size(); i < end_; ++i) {
        auto& tmp = this->gList[static_cast<size_t>(list[i])];
        auto iter = std::find_if(tmp.begin(), tmp.end(), [first](const N& other) {
            return first == other;
        });
        tmp.erase(iter);
    }

    this->edges -= list.size();
    list.clear();
}

namespace {
    size_t generate_optimal_number(const size_t &number_of_vertices, const size_t &number_of_edges) {
        size_t tmp = number_of_edges / number_of_vertices;
        if (tmp < number_of_vertices)
            return tmp + 2;
        else
            return number_of_vertices / 2;
    }
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
    if (number_of_edges > number_of_vertices * (number_of_vertices - 1))
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

    while (already_generated != number_of_edges) {
        size_t current_index = vert_dis(gen1);
        while (vertices[current_index])
            current_index = vert_dis(gen1);
        vertices[current_index] = true;

        size_t current_edges = edge_dis(gen2);
        while (already_generated + current_edges > number_of_edges) {
            current_edges = edge_dis(gen2);
        }
        already_generated += current_edges;

        std::set<size_t> tmp;
        std::vector<Node> tmp_res;
        tmp_res.reserve(current_edges);
        for (; tmp.size() < current_edges;) {
            size_t tmp_sz = tmp.size();
            size_t tmp_index = 0;
            while (tmp.size() == tmp_sz) {
                tmp_index = vert_dis(gen1);
                if (tmp_index != current_index)
                    tmp.insert(tmp_index);
            }

            ++result.inDegrees[tmp_index];
            tmp_res.emplace_back(tmp_index, weight_dis(gen0));
        }

        result.gList[current_index] = std::move(tmp_res);
    }

    return result;
}

DirectedGraph<size_t> generate_directed_graph(const size_t& number_of_vertices, const size_t& number_of_edges) {
    if (number_of_edges > number_of_vertices * (number_of_vertices - 1))
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

        std::set<size_t> tmp;
        std::vector<size_t> tmp_res;
        tmp_res.reserve(current_edges);
        for (; tmp.size() < current_edges;) {
            size_t tmp_sz = tmp.size();
            size_t tmp_index = 0;
            while (tmp.size() == tmp_sz) {
                tmp_index = vert_dis(gen1);
                if (tmp_index != current_index)
                    tmp.insert(tmp_index);
            }

            ++result.inDegrees[tmp_index];
            tmp_res.emplace_back(tmp_index);
        }

        result.gList[current_index] = std::move(tmp_res);
    }

    return result;
}

UndirectedGraph<size_t> generate_undirected_graph(const size_t& number_of_vertices, const size_t& number_of_edges) {
    if (number_of_edges > number_of_vertices * (number_of_vertices - 1) / 2)
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

        size_t initial_size = result.gList[current_index].size();
        std::set<size_t> tmp;
        if (initial_size) {
            for (const auto& it : result.gList[current_index])
                tmp.insert(it);
        }

        std::vector<size_t> tmp_res;
        tmp_res.reserve(current_edges);
        for (; tmp.size() < current_edges + initial_size;) {
            size_t tmp_sz = tmp.size();
            size_t tmp_index = 0;

            while (tmp.size() == tmp_sz) {
                tmp_index = vert_dis(gen1);
                if (tmp_index != current_index)
                    tmp.insert(tmp_index);
            }

            tmp_res.emplace_back(tmp_index);
            result.gList[tmp_index].push_back(current_index);
        }

        result.gList[current_index].insert(result.gList[current_index].end(), std::make_move_iterator(tmp_res.begin()), std::make_move_iterator(tmp_res.end()));
    }

    return result;
}