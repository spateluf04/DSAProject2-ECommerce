#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

class Graph {
    //adjacency list (for each product id, set of neighboring product ids)
    std::unordered_map<std::string, std::unordered_set<std::string>> adj;
public:
    //edge is connection between two products
    void addEdge(const std::string& a, const std::string& b);
    //returns recommended products, default number of recomendations is 3
    std::vector<std::string> suggest(const std::string& product_id, int limit = 3) const;
    int nodeCount() const;
    int edgeCount() const;
    bool hasId(const std::string& id) const;
    //set with neighbors of product
    const std::unordered_set<std::string>& neighbors(const std::string& id) const;
};
