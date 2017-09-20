#ifndef gdwg
#define gdwg
#include <list>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <queue>
#include <iostream>

namespace gdwg {
    // Edge interface
    // Denotes an edge in Graph
    // Edge contains an outgoing link connecting the node it is stored in
    // to a destination node
    // Destination is a weak pointer
    template <typename N, typename E> class Edge{
    private:
        std::weak_ptr<N> _dest;
        E _weight;
    public:
        // Constructors
        Edge(std::weak_ptr<N> dest, E weight);
        // Assignment operators
        Edge<N, E> &operator=(const Edge<N, E> &rhs);
        // Getter functions
        E getWeight() const;
        std::weak_ptr<N> getDest() const;
        // Setter
        void setDest(std::weak_ptr<N> &dest);
        // Compare edge with values
        bool isEqual(const N &dest, const E &weight);
    };
    // Node interface
    // Contains a list of edges
    template <typename N, typename E> class Node{
    private:
        std::shared_ptr<N> _node;
        std::list<Edge<N, E>> _outgoing;
        typename std::list<Edge<N, E>>::iterator findEdge(const N &dst, const E &wgt);

    public:
        // Constructors
        Node(const N node);
        // Assignment operators
        Node<N, E> &operator=(const Node<N, E> &rhs);
        // Getter functions
        int getDegree() const;
        std::weak_ptr<N> getNode() const;
        std::list<Edge<N, E>> getEdges() const;
        // Node modifying functions
        bool addEdge(const std::weak_ptr<N> &dst, const E &wgt);
        bool isConnected(const N &dst) const;
        void deleteEdge(const N &dest, const E &weight) noexcept;
        void mergeReplace(const N &old, std::weak_ptr<N> &replacement);
        void merge(const Node<N, E> &old);
        void invalidateEdges();
        // Miscellaneous functions
        void printEdges() const;
    };
    // Main Graph interface
    // Graph contains a list of nodes
    template <typename N, typename E> class Graph{
    private:
        std::list<Node<N, E>> _nodes;
        mutable typename std::list<Node<N, E>>::const_iterator _fake_iter;

        typename std::list<Node<N, E>>::iterator findNode(const N &val);
        typename std::list<Node<N, E>>::const_iterator findNode(const N &val) const;
    public:
        // Constructors
        // default move seems to work fine for the given test case
        Graph() = default;
        Graph(const Graph<N, E> &source);
        Graph(Graph<N, E> &&source) = default;
        // Assignment operators
        Graph<N, E> &operator=(const Graph<N, E> &rhs);
        Graph<N, E> &operator=(Graph<N, E> &&rhs);
        // Graph modifications
        bool addNode(const N &val);
        bool addEdge(const N &src, const N &dest, const E &weight);
        bool replace(const N &old, const N &replacement);
        void mergeReplace(const N &old, const N &replacement);
        void deleteNode(const N &target) noexcept;
        void deleteEdge(const N &src, const N &dest, const E &weight) noexcept;
        void clear() noexcept;
        // Unary functions
        bool isNode(const N &val) const;
        bool isConnected(const N &src, const N &dest) const;
        // Non modifying functions
        void printNodes() const;
        void printEdges(const N& val) const;
        // Fake iterator
        void begin() const;
        void next() const;
        bool end() const;
        const N &value() const;
    };
    #include "Graph.tem"
}
#endif
