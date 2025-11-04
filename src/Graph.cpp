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

//recommendation func
std::vector<std::string>Graph::recommend(const std::string& productId, int limit) const {
    std::vector<std::pair<std::string,int>> freq; //product,count pairs
    std::unordered_map<std::string,int> count;
    if (!adj.count(productId)) return {}; //checks to see if original product in graph
    //iterate thru neighbors of original product
    for (auto& i : adj.at(productId))
        for (auto& j : adj.at(i)) //neighbors neighbor
            if (j != productId) count[j]++;

    for (auto& p : count) freq.push_back(p); //map entries sent to vector
    std::sort(freq.begin(), freq.end(), [](auto& a, auto& b){return a.second > b.second;});
    std::vector<std::string> result;
    for (int i=0;i<limit && i<static_cast<int>(freq.size());++i) {
        result.push_back(freq[i].first);
    }
    return result;
}

//node count
int Graph::nodeCount() const {
    return adj.size();
}

//amount of edges
int Graph::edgeCount() const {
    std::size_t sum = 0;
    for (const auto& kv : adj) sum += kv.second.size();
    return sum / 2;
}

//check for valid ID
bool Graph::hasId(const std::string& id) const {
    return adj.find(id) != adj.end();
}


const std::unordered_set<std::string>& Graph::neighbors(const std::string& id) const {
    static const std::unordered_set<std::string> empty;
    std::unordered_map<std::string, std::unordered_set<std::string>>::const_iterator found = adj.find(id);

    if (it == adj.end()) {
        return empty; //empty set
    } else {
        return it->second; //neighbor set
    }
}
