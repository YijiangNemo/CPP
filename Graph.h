//
//  Graph.h
//  ass3
//
//  Created by 吴一江 on 2017/9/11.
//  Copyright © 2017年 吴一江. All rights reserved.
//

#ifndef Graph_h
#define Graph_h
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>
namespace gdwg {
    

    template <typename N, typename E> class Graph{
        
        class Graph_Node;
        class Graph_Edge;
    public:
        //use map to record the node,Node shared ptr
        std::map< N, std::shared_ptr<Graph_Node> > GNodes;
        //use vector to record the Node shared ptr for the fake iterator
        std::vector<std::shared_ptr<Graph_Node>> Iter_v;
        Graph();
        
        ~Graph();
        //copy
        Graph(const Graph &graph);
        
        //move
        Graph(Graph &&graph);
        
        //copy assignment
        Graph& operator=(const Graph &graph);
        
        //move assignment
        Graph& operator=(Graph &&graph);
        bool addNode(const N& val);
        bool addEdge(const N& src, const N& dst, const E& w);
        bool replace(const N& oldData, const N& newData);
        void mergeReplace(const N& oldData, const N& newData);
        void deleteNode(const N&) noexcept;
        void deleteEdge(const N& src, const N& dst, const E& w) noexcept;
        void clear() noexcept;
        bool isNode(const N& val) const;
        bool isConnected(const N& src, const N& dst) const;
        void printNodes() const;
        void printEdges(const N& val) const;
        
        void begin() const;
        bool end() const;
        void next() const;
        const N& value() const;
        mutable int GraphIndex = 0;
    private:
        
        class Graph_Node{
        public:
            N node;
            //two vector to store outgoing Edge and Incoming edge
            std::vector<std::shared_ptr<Graph_Edge>> Edge_list_out;
            std::vector<std::shared_ptr<Graph_Edge>> Edge_list_in;
    
            Graph_Node() { }
            
            Graph_Node(const N n): node{n} { }
            
            Graph_Node(const N n, std::shared_ptr<Graph_Edge> edge): node{n} { }
        //deconstructor
            ~Graph_Node() {
                for (auto edge : Edge_list_out){
                    edge.reset();
                }
                Edge_list_out.clear();
                for (auto edge : Edge_list_in){
                    edge.reset();
                }
                Edge_list_in.clear();
            }
            
        };
        class Graph_Edge{
        public:
            E weight;
            //two weak ptr to store the src ptr and dst ptr
            std::weak_ptr<Graph_Node> src_list;
            std::weak_ptr<Graph_Node> dst_list;
            Graph_Edge(): weight{nullptr}, src_list{nullptr}, dst_list{nullptr} { }
            
            Graph_Edge(const E w): weight{w}, src_list{nullptr}, dst_list{nullptr} { }
            
            Graph_Edge(const E w, std::shared_ptr<Graph_Node> src, std::shared_ptr<Graph_Node> dst): weight{w}, src_list{src}, dst_list{dst} {}
           //deconstructor
            ~Graph_Edge() {
                src_list.reset();
                dst_list.reset();
            }
        };
        
    };
   #include "Graph.tem" 
}

#endif /* Graph_h */
