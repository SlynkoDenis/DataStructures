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

static size_t generate_optimal_number(const size_t &number_of_vertices, const size_t &number_of_edges) {
    size_t tmp = number_of_edges / number_of_vertices;
    if (tmp + 2 < number_of_vertices)
        return tmp + 2;
    else
        return number_of_vertices / 2;
}

namespace graph {
    struct Node final {
        size_t number;
        size_t weight;

        Node() : number(0), weight(0) {};

        explicit Node(const size_t &new_number_) : number(new_number_), weight(1) {};

        Node(const size_t &new_number_, const size_t &new_weight_) : number(new_number_), weight(new_weight_) {};

        explicit operator const size_t &() const { return number; };
    };

    bool operator>=(const Node &lhs, const size_t &rhs) {
        return lhs.number >= rhs;
    }

    bool operator==(const size_t &lhs, const Node &rhs) {
        return lhs == rhs.number;
    }

    bool operator==(const Node &lhs, const size_t &rhs) {
        return lhs.number == rhs;
    }

    bool operator==(const Node &lhs, const Node &rhs) {
        return lhs.number == rhs.number && lhs.weight == rhs.weight;
    }

    std::ostream &operator<<(std::ostream &os, const Node &node) {
        os << "(" << node.number << ", " << node.weight << ")";

        return os;
    }

    template<typename N>
    class Graph {
    public:
        void reserve(size_t cap) {
            do_reserve(cap);
        };

        size_t number_of_verteces() const {
            return get_number_of_verteces();
        };

        size_t number_of_edges() const {
            return get_number_of_edges();
        };

        bool empty() const {
            return is_empty();
        };

        bool is_weighted() const {
            return check_if_weighted();
        };

        void add_node(size_t number, N *begin, size_t sz) {
            do_add_node(number, begin, sz);
        };

        void remove_edge(size_t first, const N &last) {
            do_remove_edge(first, last);
        };

        void remove_edge(size_t first) {
            do_remove_edge(first);
        };

        virtual ~Graph() noexcept = default;

    private:
        virtual void do_reserve(size_t cap) = 0;

        virtual size_t get_number_of_verteces() const = 0;

        virtual size_t get_number_of_edges() const = 0;

        virtual bool is_empty() const = 0;

        virtual bool check_if_weighted() const = 0;

        virtual void do_add_node(size_t number, N *begin, size_t sz) = 0;

        virtual void do_remove_edge(size_t first, const N &last) = 0;

        virtual void do_remove_edge(size_t first) = 0;
    };

    template<typename N>
    class DirectedGraph final : public Graph<N> {
        size_t edges;
        bool weighted;
        std::vector<std::vector<N>> adj_list;
        std::vector<size_t> in_degrees;

        void do_reserve(size_t cap) override {
            adj_list.reserve(cap);
            in_degrees.reserve(cap);
        };

        size_t get_number_of_verteces() const override {
            return adj_list.size();
        };

        size_t get_number_of_edges() const override {
            return edges;
        };

        bool is_empty() const override {
            return adj_list.empty();
        };

        bool check_if_weighted() const override {
            return weighted;
        }

        void do_add_node(size_t number, N *begin, size_t sz) override;

        void do_remove_edge(size_t first, const N &last) override;

        void do_remove_edge(size_t first) override;

    public:
        DirectedGraph() : Graph<N>(), edges(0), weighted(false) {};

        DirectedGraph(size_t sz, bool w) : Graph<N>(), edges(0), weighted(w), adj_list(sz), in_degrees(sz, 0) {};

        DirectedGraph(const DirectedGraph &other) = default;

        DirectedGraph(DirectedGraph &&other) noexcept: edges(other.edges), weighted(other.weighted),
                                                       adj_list(std::move(other.adj_list)),
                                                       in_degrees(std::move(other.in_degrees)) {
            other.edges = 0;
        };

        DirectedGraph &operator=(const DirectedGraph<N> &other) = default;

        DirectedGraph &operator=(DirectedGraph<N> &&other) noexcept {
            if (this == &other)
                return *this;

            edges = other.edges;
            weighted = other.weighted;
            adj_list = std::move(other.adj_list);
            in_degrees = std::move(other.in_degrees);

            other.edges = 0;

            return *this;
        };

        template<typename INIt>
        void add_node(size_t number, INIt begin, INIt end);

        template<typename T>
        friend std::ostream &operator<<(std::ostream &os, const DirectedGraph<T> &g);

        void generate_random_graph(size_t number_of_vertices, size_t number_of_edges, size_t max_weight = 0);

        ~DirectedGraph() override = default;
    };

    template<typename N>
    std::ostream &operator<<(std::ostream &os, const DirectedGraph<N> &g) {
        for (size_t i = 0, end_ = g.number_of_verteces() - 1; i < end_; ++i) {
            os << i << ": ";
            for (const auto &it : g.adj_list[i])
                os << it << " ";
            os << std::endl;
        }
        os << g.number_of_verteces() - 1 << ": ";
        for (const auto &it : g.adj_list[g.number_of_verteces() - 1])
            os << it << " ";

        return os;
    }

    template<typename N>
    template<typename INIt>
    void DirectedGraph<N>::add_node(size_t number, INIt begin, INIt end) {
        auto sz = std::distance(begin, end);

        if (sz < 1)
            throw std::invalid_argument("range");
        if (sz == 1)
            return;

        if (number < Graph<N>::number_of_verteces()) {
            in_degrees[number] += sz;

            auto &iter = adj_list[number];
            for (auto &it = begin; it != end; it = std::next(it)) {
                ++in_degrees[static_cast<size_t>(*it)];
                iter.push_back(*it);
            }

            edges += sz;
        } else if (number == Graph<N>::number_of_verteces()) {
            std::vector<N> tmp;
            tmp.reserve(sz);

            for (auto &it = begin; it != end; it = std::next(it)) {
                ++in_degrees[static_cast<size_t>(*it)];
                tmp.push_back(*it);
            }

            in_degrees.push_back(sz);
            adj_list.emplace_back(std::move(tmp));
            edges += sz;
        } else
            throw std::invalid_argument("vertex to add");
    }

    template<typename N>
    void DirectedGraph<N>::do_add_node(size_t number, N *begin, size_t sz) {
        if (!begin)
            throw std::invalid_argument("list of nodes");

        if (number < Graph<N>::number_of_verteces()) {
            in_degrees[number] += sz;

            auto &iter = adj_list[number];
            for (size_t i = 0; i < sz; ++i) {
                ++in_degrees[static_cast<size_t>(*(begin + i))];
                iter.push_back(*(begin + i));
            }

            edges += sz;
        } else if (number == Graph<N>::number_of_verteces()) {
            std::vector<N> tmp;
            tmp.reserve(sz);

            for (size_t i = 0; i < sz; ++i) {
                ++in_degrees[static_cast<size_t>(*(begin + i))];
                tmp.push_back(*(begin + i));
            }

            in_degrees.push_back(sz);
            adj_list.push_back(std::move(tmp));
            edges += sz;
        } else
            throw std::invalid_argument("vertex to add");
    }

    template<typename N>
    void DirectedGraph<N>::do_remove_edge(size_t first, const N &last) {
        if (first >= Graph<N>::number_of_verteces() ||
            last >= Graph<N>::number_of_verteces())      // N type must implement comparison with size_t type
            throw std::invalid_argument("invalid vertices");

        auto &list = adj_list[first];
        auto node = std::find(list.begin(), list.end(), last);
        if (node != list.end()) {
            list.erase(node);
            --in_degrees[static_cast<size_t>(last)];
            --edges;
        }
    }

    template<typename N>
    void DirectedGraph<N>::do_remove_edge(size_t first) {
        if (first >= Graph<N>::number_of_verteces())
            throw std::invalid_argument("invalid vertex");

        for (const auto &it : adj_list[first]) {
            --in_degrees[static_cast<size_t>(it)];
        }

        auto &list = adj_list[first];
        edges -= list.size();
        list.clear();
    }

    template<typename N>
    void DirectedGraph<N>::generate_random_graph(size_t number_of_vertices, size_t number_of_edges, size_t max_weight) {    // wasn't tested
        if (static_cast<uint64_t>(number_of_edges) >
            static_cast<uint64_t>(number_of_vertices) * (number_of_vertices - 1))
            throw std::invalid_argument("cannot create graph with such number of edges");

        if (max_weight) {
            DirectedGraph<N> result(number_of_vertices, true);

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
                std::fill(tmp.begin(), tmp.end(), false);
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

                    ++result.in_degrees[tmp_index];
                    tmp_res.emplace_back(tmp_index, weight_dis(gen0));
                }

                result.adj_list[current_edges] = std::move(tmp_res);
            }

            *this = std::move(result);
        } else {
            DirectedGraph<N> result(number_of_vertices, false);

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

                    ++result.in_degrees[tmp_index];
                    tmp_res.emplace_back(tmp_index);
                }

                result.adj_list[current_index] = std::move(tmp_res);
            }

            *this = std::move(result);
        }
    }

    template<typename N>
    decltype(auto)
    generate_random_directed_graph(size_t number_of_vertices, size_t number_of_edges, size_t max_weight = 0) {  // respectively wasn't tested
        DirectedGraph<N> result;
        result.generate_random_graph(number_of_vertices, number_of_edges, max_weight);
        return result;
    }

    template<typename N>
    class UndirectedGraph final : public Graph<N> {
        size_t edges;
        bool weighted;
        std::vector<std::vector<N>> adj_list;

        void do_reserve(size_t cap) override {
            adj_list.reserve(cap);
        };

        size_t get_number_of_verteces() const override {
            return adj_list.size();
        };

        size_t get_number_of_edges() const override {
            return edges;
        };

        bool is_empty() const override {
            return adj_list.empty();
        };

        bool check_if_weighted() const override {
            return weighted;
        };

        void do_add_node(size_t number, N *begin, size_t sz) override;

        void do_remove_edge(size_t first, const N &last) override;

        void do_remove_edge(size_t first) override;

    public:
        UndirectedGraph() : Graph<N>(), edges(0), weighted(false) {};

        UndirectedGraph(size_t sz, bool w) : Graph<N>(), edges(0), weighted(w), adj_list(sz) {};

        UndirectedGraph(const UndirectedGraph &other) = default;

        UndirectedGraph(UndirectedGraph &&other) noexcept: edges(other.edges), weighted(other.weighted),
                                                           adj_list(std::move(other.adj_list)) {
            other.edges = 0;
        };

        UndirectedGraph &operator=(const UndirectedGraph<N> &other) = default;

        UndirectedGraph &operator=(UndirectedGraph<N> &&other) noexcept {
            if (this == &other)
                return *this;

            auto &&ref = static_cast<UndirectedGraph<N> &&>(other);
            edges = ref.edges;
            weighted = ref.weighted;
            adj_list = std::move(ref.adj_list);

            ref.edges = 0;

            return *this;
        };

        template<typename INIt>
        void add_node(size_t number, INIt begin, INIt end);

        template<typename T>
        friend std::ostream &operator<<(std::ostream &os, const UndirectedGraph<T> &g);

        void generate_random_graph(size_t number_of_vertices, size_t number_of_edges);

        ~UndirectedGraph() override = default;
    };

    template<typename N>
    std::ostream &operator<<(std::ostream &os, const UndirectedGraph<N> &g) {
        for (size_t i = 0, end_ = g.number_of_verteces() - 1; i < end_; ++i) {
            os << i << ": ";
            for (const auto &it : g.adj_list[i])
                os << it << " ";
            os << std::endl;
        }
        os << g.number_of_verteces() - 1 << ": ";
        for (const auto &it : g.adj_list[g.number_of_verteces() - 1])
            os << it << " ";

        return os;
    }

    template<typename N>
    template<typename INIt>
    void UndirectedGraph<N>::add_node(size_t number, INIt begin, INIt end) {
        auto sz = std::distance(begin, end);

        if (sz < 1)
            throw std::invalid_argument("range");
        if (sz == 1)
            return;

        if (number < Graph<N>::number_of_verteces()) {
            auto &iter = adj_list[number];
            for (auto &it = begin; it != end; it = std::next(it)) {
                iter.push_back(*it);
                adj_list[static_cast<size_t>(*it)].push_back(static_cast<N>(number));
            }

            edges += sz;
        } else if (number == Graph<N>::number_of_verteces()) {
            std::vector<N> tmp;
            tmp.reserve(sz);

            for (auto &it = begin; it != end; it = std::next(it)) {
                tmp.push_back(*it);
                adj_list[static_cast<size_t>(*it)].push_back(static_cast<N>(number));
            }

            adj_list.emplace_back(std::move(tmp));
            edges += sz;
        } else
            throw std::invalid_argument("vertex to add");
    }

    template<typename N>
    void UndirectedGraph<N>::do_add_node(size_t number, N *begin, size_t sz) {
        if (!begin)
            throw std::invalid_argument("list of nodes");

        if (number < Graph<N>::number_of_verteces()) {
            auto &iter = adj_list[number];
            for (size_t i = 0; i < sz; ++i) {
                iter.push_back(*(begin + i));
                adj_list[static_cast<size_t>(*(begin + i))].push_back(static_cast<N>(number));
            }

            edges += sz;
        } else if (number == Graph<N>::number_of_verteces()) {
            std::vector<N> tmp;
            tmp.reserve(sz);

            for (size_t i = 0; i < sz; ++i) {
                tmp.push_back(*(begin + i));
                adj_list[static_cast<size_t>(*(begin + i))].push_back(static_cast<N>(number));
            }

            adj_list.emplace_back(std::move(tmp));
            edges += sz;
        } else
            throw std::invalid_argument("vertex to add");
    }

    template<typename N>
    void UndirectedGraph<N>::do_remove_edge(size_t first, const N &last) {
        if (first >= Graph<N>::number_of_verteces() ||
            last >= Graph<N>::number_of_verteces())      // N type must implement comparison with size_t type
            throw std::invalid_argument("invalid vertices");

        auto &list = adj_list[first];
        auto node = std::find(list.begin(), list.end(), last);

        if (node != list.end())
            list.erase(node);

        auto &last_list = adj_list[static_cast<size_t>(last)];
        node = std::find_if(last_list.begin(), last_list.end(), [first](const N &other) {
            return first == other;
        });
        if (node != last_list.end()) {
            --this->edges;
            last_list.erase(node);
        }
    }

    template<typename N>
    void UndirectedGraph<N>::do_remove_edge(size_t first) {
        if (first >= Graph<N>::number_of_verteces())
            throw std::invalid_argument("invalid vertex");

        auto &list = adj_list[first];
        for (size_t i = 0, end_ = list.size(); i < end_; ++i) {
            auto &tmp = adj_list[static_cast<size_t>(list[i])];
            auto iter = std::find_if(tmp.begin(), tmp.end(), [first](const N &other) {
                return first == other;
            });
            tmp.erase(iter);
        }

        edges -= list.size();
        list.clear();
    }

    template<typename N>
    void UndirectedGraph<N>::generate_random_graph(size_t number_of_vertices,
                                                   size_t number_of_edges) {    // was tested, must work
        if (static_cast<uint64_t>(number_of_edges) >
            static_cast<uint64_t>(number_of_vertices) * (number_of_vertices - 1) / 2)
            throw std::invalid_argument("cannot create graph with such number of edges");

        UndirectedGraph<N> result(number_of_vertices, false);

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
                        if (result.adj_list[i].size() < number_of_vertices) {
                            size_t decrement = number_of_vertices - result.adj_list[i].size();

                            std::fill(tmp_mask.begin(), tmp_mask.end(), false);
                            tmp_mask[i] = true;
                            for (auto &it : result.adj_list[i])
                                tmp_mask[it] = true;

                            size_t tmp_counter = 0;
                            while (tmp_counter != decrement) {
                                size_t tmp_index = i;
                                while (tmp_mask[i])
                                    tmp_index = vert_dis(gen1);

                                ++tmp_counter;
                                tmp_mask[tmp_index] = true;

                                result.adj_list[tmp_index].emplace_back(i);
                                result.adj_list[i].emplace_back(tmp_index);
                            }

                            current_edges -= decrement;
                        }
                    }
                }
            }

            vertices[current_index] = true;

            size_t initial_size = result.adj_list[current_index].size();

            size_t tmp_counter = 0;
            std::fill(tmp.begin(), tmp.end(), false);
            tmp[current_index] = true;

            if (initial_size) {
                for (const auto &it : result.adj_list[current_index])
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
                result.adj_list[tmp_index].emplace_back(current_index);
            }

            result.adj_list[current_index].insert(result.adj_list[current_index].end(),
                                                  std::make_move_iterator(tmp_res.begin()),
                                                  std::make_move_iterator(tmp_res.end()));
        }

        *this = std::move(result);
    }

    template<typename N>
    decltype(auto) generate_random_undirected_unweighted_graph(size_t number_of_vertices, size_t number_of_edges) {
        UndirectedGraph<N> result;
        result.generate_random_graph(number_of_vertices, number_of_edges);
        return result;
    }
}