#ifndef gdwg
#define gdwg
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <queue>
#include <iostream>

namespace gdwg {
    template <typename N, typename E> class Edge{
    private:
        std::weak_ptr<N> _dest;
        // unique ptr
        E _weight;

    public:
        Edge(std::weak_ptr<N> dest, E weight) : _dest{dest}, _weight{weight}{};
        bool isEqual(const N &dest, const E &weight);
        E getWeight() const;
        std::weak_ptr<N> getDest() const;

        Edge<N, E> &operator=(const Edge<N, E> &rhs) {
            this->_dest = rhs._dest;
            this->_weight = rhs._weight;
            return *this;
        }
    };

    template <typename N, typename E> class Node{
    private:
        std::shared_ptr<N> _node;
        std::vector<Edge<N, E>> _outgoing;

    public:
        // ‘{anonymous}::Node<int, int>& {anonymous}::Node<int, int>::operator=(const {anonymous}::Node<int, int>&)’
        Node(const N node) : Node() {
            this->_node = std::make_shared<N>(node);
        };
        Node() = default;
        Node(const Node &cpy) = default;
        Node(Node &&target) = default;
        int getDegree() const;
        std::weak_ptr<N> getNode() const;
        bool isEdge(const Edge<N, E> target) const;
        void addEdge(const std::weak_ptr<N> &dst, const E &wgt);
        void printEdges() const;

        Node<N, E> &operator=(const Node<N, E> &rhs){
            this->_node = rhs._node;
            this->_outgoing = rhs._outgoing;
            return *this;
        };

        typename std::vector<Edge<N, E>>::iterator findEdge(const N &dst, const E &wgt);
        typename std::vector<Edge<N, E>>::iterator edgeEnd();
    };


    template <typename N, typename E> class Graph{
        // members
    private:
        std::vector<Node<N, E>> _nodes;

        typename std::vector<Node<N, E>>::iterator findNode(const N &val);
        typename std::vector<Node<N, E>>::const_iterator findNode(const N &val) const;
    public:

        Graph() = default;
        Graph(const Graph &source) = default;
        Graph(Graph &&source) = default;

        bool addNode(const N &val);
        bool addEdge(const N &src, const N &dest, const E &weight);
        bool replace(const N &old, const N &replacement);
        void mergeReplace(const N &old, const N &replacement);
        void deleteNode(const N &target) noexcept;
        void deleteEdge(const N &src, const N &dest, const E &weight) noexcept;
        void clear() noexcept;
        bool isNode(const N &val) const;
        bool isConnected(const N &src, const N &dest) const;
        void printNodes() const;
        void printEdges(const N& val) const;
    };
    // Node class functions
    template <typename N, typename E>
    int Node<N, E>::getDegree() const{
        return this->_outgoing.size();
    }

    template <typename N, typename E>
    std::weak_ptr<N> Node<N, E>::getNode() const{
        return this->_node;
    }

    // Comparator for nodes, so priority queue works
    template <typename N, typename E>
    bool operator<(const Node<N, E> &lhs, const Node<N, E> &rhs){
        bool isLess = (lhs.getDegree() < rhs.getDegree());
        if(!isLess) return (*(lhs.getNode().lock()) < *(rhs.getNode().lock()));
        return isLess;
    }


    // Edge class functions
    template <typename N, typename E>
    bool operator<(const Edge<N, E> &lhs, const Edge<N, E> &rhs) {
        return lhs.getWeight() < rhs.getWeight();
    }

    template <typename N, typename E>
    E Edge<N, E>::getWeight() const {
        return this->_weight;
    }

    template <typename N, typename E>
    std::weak_ptr<N> Edge<N, E>::getDest() const {
        return this->_dest;
    }

    // Graph class functions
    // Add node to Graph
    // Returns true if node added, returns false if node already exist
    template <typename N, typename E>
    bool Graph<N, E>::addNode(const N &val) {
        auto res = this->findNode(val);
        bool suc = (res == this->_nodes.end());
        if(suc){
            Node<N, E> new_node{val};
            this->_nodes.push_back(new_node);
        }
        return suc;
    }

    // Add edge to Graph
    // Returns false if edge already exist. True otherwise.
    template <typename N, typename E>
    bool Graph<N, E>::addEdge(const N &src, const N &dest, const E &weight){
        // find src node
        auto s_res   = this->findNode(src), d_res = this->findNode(dest);
        // false = found, true = not found
        auto found =
            (s_res == this->_nodes.end()) || (d_res == this->_nodes.end());
        if(found){
            //TODO: throw error
        }
        auto e_res   = (*s_res).findEdge(dest, weight);
        found = (e_res == (*s_res).edgeEnd());
        if(found){
            (*s_res).addEdge((*d_res).getNode(), weight);
        }
        return found;
    }

    template <typename N, typename E>
    bool Graph<N, E>::isNode(const N &val) const {
        auto res = this->findNode(val);
        return (res != this->_nodes.end());
    }

    template <typename N, typename E>
    bool Graph<N, E>::isConnected(const N &src, const N &dest) const {
        auto s_res = this->findNode(src), d_res = this->findNode(dest);
        auto found =
            (s_res == this->_nodes.cend()) || (d_res == this->_nodes.cend());
        if(found)
            throw std::runtime_error("Source or Destination not in Graph.\n");
        //TODO:
        return false;
    }

    // prints nodes
    template <typename N, typename E>
    void Graph<N, E>::printNodes() const{
        auto cpy = this->_nodes;
        std::sort(cpy.begin(), cpy.end());
        for(auto i = 0u; i < cpy.size(); ++i){
            std::cout << *(cpy[i].getNode().lock()) << "\n";
        }
    }

    // print the outgoing edges of a given node
    template <typename N, typename E>
    void Graph<N, E>::printEdges(const N& val) const{
        auto res = std::find_if(this->_nodes.cbegin(), this->_nodes.cend(),
            [val](auto &sp) {
                return *(sp.getNode().lock()) == val;
            }
        );
        if(res == this->_nodes.cend())
            std::runtime_error("Source doesn't exist");
        (*res).printEdges();
    }

    template <typename N, typename E>
    void Node<N, E>::printEdges() const {
        std::cout << "Edges attached to Node " << *this->_node << "\n";
        auto cpy = this->_outgoing;
        if(cpy.size() == 0) {
            std::cout << "(null)\n";
            return;
        }
        std::sort(cpy.begin(), cpy.end());
        for(auto i : cpy) {
            std::cout << *(i.getDest().lock()) << " " << i.getWeight() << "\n";
        }
    }
    // Find a specific node in a graph and return the iterator
    template <typename N, typename E>
    typename std::vector<Node<N, E>>::iterator Graph<N, E>::findNode(const N &node) {
        // find iterator containing node
        auto res = std::find_if(this->_nodes.begin(), this->_nodes.end(),
            [node](auto &sp) {
                return *(sp.getNode().lock()) == node;
            }
        );
        return res;
    }

    template <typename N, typename E>
    typename std::vector<Node<N, E>>::const_iterator Graph<N, E>::findNode(const N &node) const {
        // find iterator containing node
        auto res = std::find_if(this->_nodes.cbegin(), this->_nodes.cend(),
            [node](auto &sp) {
                return *(sp.getNode().lock()) == node;
            }
        );
        return res;
    }
    // Find an outgoing edge from a node
    template <typename N, typename E>
    typename std::vector<Edge<N, E>>::iterator Node<N, E>::findEdge(const N &dst, const E &wgt) {
        // find iterator containing node
        auto res = std::find_if(this->_outgoing.begin(), this->_outgoing.end(),
            [dst, wgt](auto &sp) {
                return sp.isEqual(dst, wgt);
            }
        );
        return res;
    }

    template <typename N, typename E>
    typename std::vector<Edge<N, E>>::iterator Node<N, E>::edgeEnd() {
        return this->_outgoing.end();
    }

    template <typename N, typename E>
    bool Edge<N, E>::isEqual(const N &dest, const E &weight){
        return (*this->_dest.lock() == dest) && (this->_weight == weight);
    }
    // Finds a specific edge in a graph and return the iterator
    template <typename N, typename E>
    void Node<N, E>::addEdge(const std::weak_ptr<N> &dst, const E &wgt){
        Edge<N, E> new_edge{dst, wgt};
        this->_outgoing.push_back(new_edge);
    }

}
#endif
