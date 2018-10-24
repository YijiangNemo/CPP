///////////////////////////////////////////////////////////////
//    														  //
//                         ###    ######   ######      ##    //
//                        ##      ##  ##   ##  ##     ###    //
//      #####    #####   ##           ##       ##      ##    //
//     ##       ##       #####       ##       ##       ##    //
//     ##        ####    ##  ##      ##       ##       ##    //
//     ##           ##   ##  ##     ##       ##        ##    //
//      #####   #####     ####      ##       ##        ##    //
//    														  //
//     		COMP6771 Assignment_3: SmartPointers		  //
//     				TEMPLATE FILE					  	  //
//    														  //
///////////////////////////////////////////////////////////////


template <typename N, typename E>
class Graph
{
    private:
    struct Node;
    struct Edge;
    struct Node
    {
        N val_;
        int numEdges_;
        std::set<std::shared_ptr<Edge>> edges_;
        Node() {}
        Node(const N x) : val_{x} {numEdges_=0;}
        void printNode (N n) {
            if (n!=val_) throw std::runtime_error("Node corruption: index != val_");
            std::cout << val_ << ": ";
            for (std::shared_ptr<Edge> e : edges_) (*e).printEdge();
            std::cout << std::endl;
        }
        ~Node() {
            
            edges_.clear();
        }
        void update () {
            for (auto it = edges_.begin(); it != edges_.end();) {
                if (!((*it)->dest.lock()))	edges_.erase(it++);
                else ++it;
            }
            numEdges_ = edges_.size();
        }
    };
    
    struct Edge
    {
        std::weak_ptr<Node> orig;
        std::weak_ptr<Node> dest;
        E val_;
        Edge(std::shared_ptr<Node> o, std::shared_ptr<Node> d, E x) : val_{x} {
            orig = o;
            dest = d;
        }
        Edge() {};
        void printEdge () {
            if (auto tmp = dest.lock())
            std::cout << "[" << val_ << "] " << tmp->val_ << " ";
        }
        ~Edge() {
            
            orig.reset();
            dest.reset();
        }
    };
    
    public:
    friend class Node_Iterator<N, E>;
    friend class Edge_Iterator<N, E>;
    mutable Node_Iterator<N, E> curr;
    Graph() :
				nodes_{}
				{};
    
    //copy constructor
    Graph(const Graph &g) :
				nodes_{g.nodes_}
				{};
    
    //move constructor
    Graph(Graph &&g) :
				nodes_{std::move(g.nodes_)}
				{};
    
    //Destructor
    ~Graph()
    {
        
        nodes_.clear();
    };
    
    //=copy Operator (deep copy)
    Graph& operator=(const Graph &g)
    {
        
        if (nodes_ != g.nodes_) {
            nodes_.erase(nodes_.begin(),nodes_.end());
            for (auto node : g.nodes_) {
                nodes_[node.second->val_] = std::make_shared<Node>(Node{node.second->val_});
            }
            // add all the edges
            for (auto node : g.nodes_) {
                for (auto edge : node.second->edges_) {
                    
                    // must use successive if statements because of auto type deduction
                    if (auto tmp1 = edge->dest.lock()) {
                        auto orig = nodes_.find(tmp1->val_);
                        if (auto tmp2 = edge->dest.lock()) {
                            auto dest = nodes_.find(tmp2->val_);
                            nodes_[node.second->val_]->edges_.insert(std::make_shared<Edge>(Edge(orig->second,dest->second,edge->val_)));
                        }
                    }
                }
            }
        }
        return *this;
    }
    
    //=move Operator
    Graph& operator=(Graph &&g)
    {
        
        if ((*this).nodes_ != g.nodes_) {
            nodes_ = std::move(g.nodes_);
        }
        return *this;
    }
    
    bool addNode(const N& val_)
    {
        if (nodes_.find(val_) == nodes_.end()) {
            
            nodes_[val_] = std::make_shared<Node>(Node{val_});
        } else {
            
            return false;
        }
        
        return true;
    }
    
    bool addEdge(const N& orig, const N& dest, const E& val)
    {
        // check that both orig and dest exist
        auto findOrig = nodes_.find(orig);
        if (findOrig == nodes_.end()) throw std::runtime_error("addEdge: origin Does not exist");
        auto findDest = nodes_.find(dest);
        if (findDest == nodes_.end()) throw std::runtime_error("addEdge: dest Does not exist");
        
        // check if the edge between already exists
        // find if the edge already exists (i.e an existing edge with the same value AND destination)
        auto findEdge = std::find_if(findOrig->second->edges_.begin(),findOrig->second->edges_.end(),
                                     [val,dest] (const std::shared_ptr<Edge>& e) {
                                         if (!(e->val_ < val) && !(val < e->val_))
                                         if (auto tmp = e->dest.lock())
                                         if (!(tmp->val_ < dest) && !(dest < tmp->val_))
                                         return true;
                                         return false;
                                     });
        
        if (findEdge == findOrig->second->edges_.end()) {
            
            findOrig->second->edges_.insert(std::make_shared<Edge>(Edge(findOrig->second,findDest->second,val)));
            ++findOrig->second->numEdges_;
            return true;
            
            std::cout << orig << "->" << dest << " not added. Already exists" << std::endl;
        }
        return false;
    }
    
    // replace the val in node with the new val
    bool replace(const N& node, const N& val)
    {
        // check the node exists
        auto findNode = nodes_.find(node);
        if (findNode == nodes_.end()) throw std::runtime_error("replace: Node Does not exist");
        
        // check if val already exists
        auto findval_ = nodes_.find(val);
        
        if (findval_ == nodes_.end()) {
            
            nodes_[val] = nodes_[node];
            nodes_[val]->val_ = val;
            nodes_.erase(findNode);
            //findNode->second->val_ = 66;
            return true;
        }
        
        return false;
    }
    
    void mergeReplace(const N& oldNode, const N& newNode) {
        // if either the old or new node DNE, throw an error
        auto findOld = nodes_.find(oldNode);
        if (findOld == nodes_.end()) throw std::runtime_error("mergeReplace: old Does not exist");
        auto findNew = nodes_.find(newNode);
        if (findNew == nodes_.end()) throw std::runtime_error("mergeReplace: new Does not exist");
        
        // for each edge in oldNode
        for (auto edge : findOld->second->edges_) {
            // if it still exists
            if (auto lockedDest = edge->dest.lock()) {
                // check it isn't an edge between old and new
                if (!(lockedDest->val_ < newNode) && !(newNode < lockedDest->val_)) {
                    continue;
                }
                // check if it exists in the new node
                E val = edge->val_;
                N dest = lockedDest->val_;
                auto findEdge = std::find_if(findNew->second->edges_.begin(),findNew->second->edges_.end(),
                                             [&val,&dest] (const std::shared_ptr<Edge>& e) {
                                                 if (!(e->val_ < val) && !(val < e->val_))
                                                 if (auto tmp = e->dest.lock())
                                                 if (!(tmp->val_ < dest) && !(dest < tmp->val_))
                                                 return true;
                                                 return false;
                                             });
                
                // if it doesn't, add it
                if (findEdge == findNew->second->edges_.end()) {
                    findNew->second->edges_.insert(std::make_shared<Edge>(Edge(findNew->second,lockedDest,edge->val_)));
                    ++findNew->second->numEdges_;
                    
                    std::cout << "In mergeReplace: " << newNode << "->" << dest << " val: " << edge->val_ << " not added. Already exists" << std::endl;
                }
                // else go to next edge
            } else continue;
        }
        deleteNode (oldNode);
    }
    
    void deleteNode(const N& node) noexcept
    {
        auto findNode = nodes_.find(node);
        if (findNode != nodes_.end()) {
            
            findNode->second.reset();
            nodes_.erase(findNode);
        } else {
            
        }
    }
    
    void deleteEdge(const N& orig, const N& dest, const E& val) noexcept
    {
        // find the origin node
        auto findNode = nodes_.find(orig);
        // find the destination node: Must have same dest AND val_
        if (findNode != nodes_.end()) {
            auto findEdge = std::find_if(findNode->second->edges_.begin(), findNode->second->edges_.end(),
                                         [dest, val] (const std::shared_ptr<Edge>& e) {
                                             if (!(e->val_ < val) && !(val < e->val_))
                                             if (auto tmp = e->dest.lock())
                                             if (!(tmp->val_ < dest) && !(dest < tmp->val_))
                                             return true;
                                             return false;
                                         });
            if (findEdge != findNode->second->edges_.end()) {
                findNode->second->edges_.erase(findEdge);
                --findNode->second->numEdges_;
            }
        }
    }
    
    void clear() noexcept
    {
        nodes_.clear();
    }
    
    bool isNode(const N& node) const
    {
        return nodes_.find(node) != nodes_.end();
    }
    
    bool isConnected(const N& orig, const N& dest) const
    {
        auto findOrig = nodes_.find(orig);
        if (findOrig == nodes_.end()) throw std::runtime_error("addEdge: origin Does not exist");
        auto findDest = nodes_.find(dest);
        if (findDest == nodes_.end()) throw std::runtime_error("addEdge: dest Does not exist");
        
        // find if the edge exists in orig to dest
        auto findEdge = std::find_if(findOrig->second->edges_.begin(), findOrig->second->edges_.end(),
                                     [dest] (const std::shared_ptr<Edge>& e) {
                                         if (auto tmp = e->dest.lock())
                                         if (!(tmp->val_ < dest) && !(dest < tmp->val_))
                                         return true;
                                         return false;
                                     });
        return findEdge != findOrig->second->edges_.end()? true : false;
    }
    
    /*
     * Prints the nodes in descending number of edges order
     */
    
    void printNodes() const
    {
        
        for (auto it =Internal_Begin(); it !=Internal_End(); ++it ) {
            std::cout << *it << std::endl;
        }
    }
    
    
    void printEdges(const N& node) const
    {
        // check if the node exists
        auto findOrig = nodes_.find(node);
        if (findOrig == nodes_.end()) throw std::runtime_error("printEdges: node Does not exist");
        std::cout << "Edges attached to Node " << node << std::endl;
        bool tmp = false;
        for (auto it = edgeIteratorBegin(node); it != edgeIteratorEnd(node); ++it) {
            //std::cout << findOrig->second->edges_.size() << std::endl;
            std::cout << (*it).first << " " << (*it).second << std::endl;
            tmp = true;
        }
        if (!tmp) std::cout << "(null)" << std::endl;
    }
    
    
    void printGraph () const
    {
        std::cout << "Printing Graph" << std::endl;
        for (auto n : nodes_) {
            n.second->printNode(n.first);
        }
    }
    
    
    Node_Iterator<N, E> Internal_Begin() const
    {
        return Node_Iterator<N, E>(&nodes_);
    };
    
    
    Node_Iterator<N, E> Internal_End() const
    {
        return Node_Iterator<N, E>(nullptr);
    }
    
    
    void next() const
    {
        ++curr;
    };
    
    
    void begin() const{
        curr = Node_Iterator<N, E>(true, &nodes_);
    }
    
    
    bool end() const{
        if(curr == Node_Iterator<N, E>(true, nullptr)) return 1;
        else return 0;
    }
    
    
    const N& value() const
    {
        return *curr;
    };
    
    
    Edge_Iterator<N, E> edgeIteratorBegin(const N& node) const
    {
        auto findNode = nodes_.find(node);
        if (findNode != nodes_.end()) {
            return Edge_Iterator<N, E>(findNode->second);
        }
        throw std::runtime_error("edgeIteratorBegin: edge Does not exist");
    }
    
    
    Edge_Iterator<N, E> edgeIteratorEnd (const N& node) const 
    {
        return Edge_Iterator<N, E>(nullptr);
    }
    
    
    Edge_Iterator<N, E> edgeIteratorEnd () const 
    {
        return Edge_Iterator<N, E>(nullptr);
    }
    
    
    private:
    std::map< N, std::shared_ptr<Node> > nodes_;
};


