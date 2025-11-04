#include "Graph.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

//undirected edge used to represent if products were purchased together
void Graph::addEdge(const std::string & a, const std::string & b) {
    if (a == b) {
        return;
    }
    adj[a].insert(b);
    adj[b].insert(a);
}

//
// Created by Kush Patel on 11/3/25.
//
