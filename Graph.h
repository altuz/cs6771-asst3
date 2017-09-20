#ifndef gdwg
#define gdwg
#include <list>
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
        void setWeight(E &weight);
        void setDest(std::weak_ptr<N> &dest);
        Edge<N, E> &operator=(const Edge<N, E> &rhs) {
            this->_dest = rhs._dest;
            this->_weight = rhs._weight;
            return *this;
        }
    };

    template <typename N, typename E> class Node{
    private:
        std::shared_ptr<N> _node;
        std::list<Edge<N, E>> _outgoing;

    public:
        // ‘{anonymous}::Node<int, int>& {anonymous}::Node<int, int>::operator=(const {anonymous}::Node<int, int>&)’
        Node(const N node) : Node() {
            this->_node = std::make_shared<N>(node);
        };
        Node() = default;
        Node(const Node<N, E> &cpy) = default;
        Node(Node<N, E> &&target) = default;
        int getDegree() const;
        std::weak_ptr<N> getNode() const;
        bool isConnected(const N &dst) const;
        void addEdge(const std::weak_ptr<N> &dst, const E &wgt);
        void printEdges() const;
        void deleteEdge(const N &dest, const E &weight) noexcept;
        void mergeReplace(const N &old, std::weak_ptr<N> &replacement);
        void merge(const Node<N, E> &old);
        void invalidateEdges();
        std::list<Edge<N, E>> getEdges() const;

        Node<N, E> &operator=(const Node<N, E> &rhs){
            this->_node = rhs._node;
            this->_outgoing = rhs._outgoing;
            return *this;
        };

        typename std::list<Edge<N, E>>::iterator findEdge(const N &dst, const E &wgt);
        typename std::list<Edge<N, E>>::iterator edgeEnd();
    };


    template <typename N, typename E> class Graph{
        // members
    private:
        std::list<Node<N, E>> _nodes;
        mutable typename std::list<Node<N, E>>::const_iterator _fake_iter;

        typename std::list<Node<N, E>>::iterator findNode(const N &val);
        typename std::list<Node<N, E>>::const_iterator findNode(const N &val) const;
    public:
        Graph() = default;
        Graph(const Graph<N, E> &source);
        Graph(Graph<N, E> &&source) = default;

        Graph<N, E> &operator=(const Graph<N, E> &rhs);
        Graph<N, E> &operator=(Graph<N, E> &&rhs);
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
        void begin() const;
        void next() const;
        bool end() const;
        const N &value() const;
    };
    //////////////////////////
    // Node class functions //
    //////////////////////////
    // Returns the number of outgoing edges
    template <typename N, typename E>
    int Node<N, E>::getDegree() const{
        return this->_outgoing.size();
    }
    // Returns a weak_ptr to the node value
    template <typename N, typename E>
    std::weak_ptr<N> Node<N, E>::getNode() const{
        return this->_node;
    }
    // Comparator for nodes, so priority queue works
    template <typename N, typename E>
    bool operator<(const Node<N, E> &lhs, const Node<N, E> &rhs){
        auto l_deg = lhs.getDegree(), r_deg = rhs.getDegree();
        if(l_deg == r_deg) return (*(lhs.getNode().lock()) < *(rhs.getNode().lock()));
        return l_deg < r_deg;
    }
    // Prints all the edges in current node
    template <typename N, typename E>
    void Node<N, E>::printEdges() const {
        std::cout << "Edges attached to Node " << *this->_node << "\n";
        auto cpy = this->_outgoing;
        if(cpy.size() == 0) {
            std::cout << "(null)\n";
            return;
        }
        cpy.sort();
        // std::sort(cpy.begin(), cpy.end());
        for(auto i : cpy) {
            std::cout << *(i.getDest().lock()) << " " << i.getWeight() << "\n";
        }
    }
    // Checks if current node is conected to dest
    template <typename N, typename E>
    bool Node<N, E>::isConnected(const N &dest) const {
        for(auto &e : this->_outgoing){
            if(*e.getDest().lock() == dest)
                return true;
        }
        return false;
    }
    // Find an outgoing edge from a node
    template <typename N, typename E>
    typename std::list<Edge<N, E>>::iterator Node<N, E>::findEdge(const N &dst, const E &wgt) {
        // find iterator containing node
        auto res = std::find_if(this->_outgoing.begin(), this->_outgoing.end(),
            [dst, wgt](auto &sp) {
                return sp.isEqual(dst, wgt);
            }
        );
        return res;
    }
    // Finds a specific edge in a graph and return the iterator
    template <typename N, typename E>
    void Node<N, E>::addEdge(const std::weak_ptr<N> &dst, const E &wgt){
        Edge<N, E> new_edge{dst, wgt};
        this->_outgoing.push_back(new_edge);
    }
    // Returns the end of iterator
    template <typename N, typename E>
    typename std::list<Edge<N, E>>::iterator Node<N, E>::edgeEnd() {
        return this->_outgoing.end();
    }
    // Delete an edge from the current node
    template <typename N, typename E>
    void Node<N, E>::deleteEdge(const N &dest, const E &weight) noexcept {
        auto res = this->findEdge(dest, weight);
        if(res == this->_outgoing.end())
            return;
        this->_outgoing.erase(res);
    }
    // Replace any incoming edge going to old to point to replacement
    template <typename N, typename E>
    void Node<N, E>::mergeReplace(const N &old, std::weak_ptr<N> &replacement) {
        for (auto &i : this->_outgoing) {
            auto ptr = i.getDest();
            if (*(ptr.lock()) == old)
                i.setDest(replacement);
        }
    }
    // Merge outgoing edge from old to this
    template <typename N, typename E>
    void Node<N, E>::merge(const Node<N, E> &old) {
        this->_outgoing.insert(this->_outgoing.end(), old._outgoing.begin(), old._outgoing.end());
        this->_outgoing.sort();
        this->_outgoing.unique();
    }
    // Invalidate edges
    template <typename N, typename E>
    void Node<N, E>::invalidateEdges() {
        // std::cout << "before size is: " << this->_outgoing.size() << "\n";
        this->_outgoing.remove_if(
            [] (auto &i) {
                return i.getDest().expired();
            }
        );
        // std::cout << "after size is: " << this->_outgoing.size() << "\n";
    }
    // Get all edges
    template <typename N, typename E>
    std::list<Edge<N, E>> Node<N, E>::getEdges() const {
        return this->_outgoing;
    }
    //////////////////////////
    // Edge class functions //
    //////////////////////////
    // < Comparator for edges
    template <typename N, typename E>
    bool operator<(const Edge<N, E> &lhs, const Edge<N, E> &rhs) {
        return lhs.getWeight() < rhs.getWeight();
    }
    // == Comparator for edges
    template <typename N, typename E>
    bool operator==(const Edge<N, E> &lhs, const Edge<N, E> &rhs) {
        bool eq = (lhs.getWeight() == rhs.getWeight());
        eq = eq && (lhs.getDest().lock() == rhs.getDest().lock());
        return eq;
    }
    // Return the weight of current edge
    template <typename N, typename E>
    E Edge<N, E>::getWeight() const {
        return this->_weight;
    }
    // Returns a weak pointer to the destination of the edge
    template <typename N, typename E>
    std::weak_ptr<N> Edge<N, E>::getDest() const {
        return this->_dest;
    }
    // Checks for equality of two edges
    template <typename N, typename E>
    bool Edge<N, E>::isEqual(const N &dest, const E &weight){
        return (*this->_dest.lock() == dest) && (this->_weight == weight);
    }
    // Sets edge dest to new dest
    template <typename N, typename E>
    void Edge<N, E>::setDest(std::weak_ptr<N> &dest) {
        this->_dest = dest;
    }

    template <typename N, typename E>
    void Edge<N, E>::setWeight(E &weight) {
        this->_weight = weight;
    }

    ///////////////////////////
    // Graph class functions //
    ///////////////////////////
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
            throw std::runtime_error("Source or Destination not in Graph.\n");
        }
        auto e_res   = (*s_res).findEdge(dest, weight);
        found = (e_res == (*s_res).edgeEnd());
        if(found){
            (*s_res).addEdge((*d_res).getNode(), weight);
        }
        return found;
    }
    // Checks if a node exists in graph
    template <typename N, typename E>
    bool Graph<N, E>::isNode(const N &val) const {
        auto res = this->findNode(val);
        return (res != this->_nodes.end());
    }
    // Checks if there is an edge between two nodes (src -> dest)
    template <typename N, typename E>
    bool Graph<N, E>::isConnected(const N &src, const N &dest) const {
        auto s_res = this->findNode(src), d_res = this->findNode(dest);
        auto found =
            (s_res == this->_nodes.cend()) || (d_res == this->_nodes.cend());
        if(found)
            throw std::runtime_error("Source or Destination not in Graph.\n");
        return (*s_res).isConnected(dest);
    }
    // Replace old data with new data
    template <typename N, typename E>
    bool Graph<N, E>::replace(const N &old, const N &replacement) {
        auto o_res = this->findNode(old), n_res = this->findNode(replacement);
        auto o_found = (o_res == this->_nodes.end());
        if(o_found)
            throw std::runtime_error("Source or Destination not in Graph.\n");
        auto n_found = (n_res == this->_nodes.end());
        if(n_found){
            auto old = *o_res;
            auto old_ptr = old.getNode().lock();
            *old_ptr = replacement;
        }
        return n_found;
    }
    // prints nodes
    template <typename N, typename E>
    void Graph<N, E>::printNodes() const{
        auto cpy = this->_nodes;
        cpy.sort();
        //std::sort(cpy.begin(), cpy.end());
        // for(auto i = 0u; i < cpy.size(); ++i){
        //     std::cout << *(cpy[i].getNode().lock()) << "\n";
        // }
        for (auto i : cpy) {
            std::cout << *(i.getNode().lock()) << "\n";
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
            throw std::runtime_error("Source doesn't exist");
        (*res).printEdges();
    }
    // Find a specific node in a graph and return the iterator
    template <typename N, typename E>
    typename std::list<Node<N, E>>::iterator Graph<N, E>::findNode(const N &node) {
        // find iterator containing node
        auto res = std::find_if(this->_nodes.begin(), this->_nodes.end(),
            [node](auto &sp) {
                return *(sp.getNode().lock()) == node;
            }
        );
        return res;
    }
    // const version of the above
    template <typename N, typename E>
    typename std::list<Node<N, E>>::const_iterator Graph<N, E>::findNode(const N &node) const {
        // find iterator containing node
        auto res = std::find_if(this->_nodes.cbegin(), this->_nodes.cend(),
            [node](auto &sp) {
                return *(sp.getNode().lock()) == node;
            }
        );
        return res;
    }
    // Merge and replace
    template <typename N, typename E>
    void Graph<N, E>::mergeReplace(const N &old, const N &replacement) {
        auto s_res = this->findNode(old), d_res = this->findNode(replacement);
        auto found =
            (s_res == this->_nodes.end()) || (d_res == this->_nodes.end());
        if (found)
            throw std::runtime_error("Source or Destination not in Graph.\n");
        // Change all incoming edges to old to point to replacement
        for (auto &i : this->_nodes){
            auto d_weak = (*d_res).getNode();
            i.mergeReplace(old, d_weak);
        }
        // Change all outgoing edges from old to point from replacement
        (*d_res).merge(*s_res);
        this->_nodes.erase(s_res);
    }
    // Delete a node
    template <typename N, typename E>
    void Graph<N, E>::deleteNode(const N &target) noexcept {
        auto res = this->findNode(target);
        if(res == this->_nodes.end())
            return;
        this->_nodes.erase(res);
        // TODO: check for invalid edges during printing
        for (auto &i : this->_nodes)
            i.invalidateEdges();
    }
    // Delete an edge
    template <typename N, typename E>
    void Graph<N, E>::deleteEdge(const N &src, const N &dest, const E &weight) noexcept {
        auto s_res = this->findNode(src), d_res = this->findNode(dest);
        auto found =
            (s_res == this->_nodes.end()) || (d_res == this->_nodes.end());
        if (found)
            return;
        (*s_res).deleteEdge(dest, weight);
    }
    // Clears graph of all nodes and edges
    template <typename N, typename E>
    void Graph<N, E>::clear() noexcept {
        this->_nodes.erase(this->_nodes.begin(), this->_nodes.end());
    }
    // Copy constructor for graph
    template <typename N, typename E>
    Graph<N, E>::Graph(const Graph<N, E> &source) : Graph() {
        std::list<std::tuple<N, N, E>> all_edges;
        for (auto &i : source._nodes) {
            auto n_val = *(i.getNode().lock());
            auto n_edges = i.getEdges();
            this->addNode(n_val);
            for (auto &j : n_edges) {
                auto d_val = *(j.getDest().lock());
                auto w_val = j.getWeight();
                all_edges.push_back(std::make_tuple(n_val, d_val, w_val));
            }
        }

        for (auto &i : all_edges) {
            auto src = std::get<0>(i), dest = std::get<1>(i);
            auto weight = std::get<2>(i);
            this->addEdge(src, dest, weight);
        }
    }
    // Copy assignment
    template <typename N, typename E>
    Graph<N, E> &Graph<N, E>::operator=(const Graph<N, E> &rhs) {
        Graph<N, E> cpy{rhs};
        std::swap(this->_nodes, cpy._nodes);
        return *this;
    }
    // Move assignment
    template <typename N, typename E>
    Graph<N, E> &Graph<N, E>::operator=(Graph<N, E> &&rhs) {
        Graph<N, E> cpy{std::move(rhs)};
        std::swap(this->_nodes, cpy._nodes);
        return *this;
    }
    // Start of iterator
    template <typename N, typename E>
    void Graph<N, E>::begin() const {
        this->_fake_iter = this->_nodes.cbegin();
    }
    // Move iterator
    template <typename N, typename E>
    void Graph<N, E>::next() const {
        this->_fake_iter++;
    }
    // Check if iterator is at end of list
    template <typename N, typename E>
    bool Graph<N, E>::end() const {
        return (this->_fake_iter == this->_nodes.cend());
    }
    // Get value at iterator
    template <typename N, typename E>
    const N &Graph<N, E>::value() const {
        return *(*this->_fake_iter).getNode().lock();
    }
}
#endif
