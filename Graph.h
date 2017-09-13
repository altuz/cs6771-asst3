#ifndef gdwg
#define gdwg
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
namespace gdwg {
    template <typename N, typename E> class Graph{
        // members
        private:
        class Edge{
            // use weak ptr
            std::weak_ptr<N> dest;
            // unique ptr
            std::unique_ptr<E> weight;
            
            bool operator==(Edge &rhs){
                return (dest == rhs.dest) && (weight == rhs.weight);
            }
        };
        
        // shared ptr
        std::vector<std::shared_ptr<N>> nodes;
        // weak ptr for key
        // how to compare weak ptr tho
        std::unordered_map<std::weak_ptr<N>, std::vector<Edge>> edges;
        
        public:
        
        Graph();
        Graph(const Graph &source);
        Graph(Graph &&source);
        
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
        void printEdges() const;
    };
    #include "Graph.tem"

    template <typename N, typename E> Graph<N, E>::Graph() : nodes{{}}, edges{{}} {}
    template <typename N, typename E> Graph<N, E>::Graph(const Graph &source) : Graph{} {}
    template <typename N, typename E> Graph<N, E>::Graph(Graph &&source) : Graph{} {}
    
    template <typename N, typename E> bool Graph<N, E>::addNode(const N &val) {
        // try to find val
        auto res = std::find_if(this->nodes.cbegin, this->nodes.cend, [val](auto &sp) {
            return *(sp.lock()) == val;
        });
        // emplace val into nodes if not found
        auto found = (res != this->nodes.cend);
        if (!found){
            this->nodes.emplace(val);
        }
        return found;
    }
    
    template <typename N, typename E> bool Graph<N, E>::addEdge(const N &src, const N &dest, const E &weight) {
        Edge a{dest, weight};
    }
}
#endif
