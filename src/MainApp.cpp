#include "MainApp.h"
#include "DataLoader.h"
#include "DataStats.h"
#include "Sort.h"
#include "Graph.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdlib>

namespace ecomm {

    void MainApp::run() {
        int choice = -1;

        while (true) {
            std::cout << "\n==== E-COMMERCE DSA MENU ====\n"
                      << "1. Load Data\n"
                      << "2. Show Statistics\n"
                      << "3. Sort Benchmark\n"
                      << "4. Build Graph\n"
                      << "5. Query Graph\n"
                      << "0. Exit\n> ";

            //Reads and validates menu choices
            std::string input;
            std::getline(std::cin, input);
            if (input.empty()) continue;
            try {
                choice = std::stoi(input);
            } catch (...) {
                std::cout << "Invalid input.\n";
                continue;
            }

            //Exit path
            if (choice == 0) {
                std::cout << "Exiting...\n";
                break;
            }

            //Path to load data
            switch (choice) {
                case 1: {
                    std::string order_items, orders, products, customers;
                    std::cout << "Order ITEMS CSV path: ";
                    std::getline(std::cin, order_items);
                    std::cout << "Orders CSV path: ";
                    std::getline(std::cin, orders);
                    std::cout << "Products CSV path: ";
                    std::getline(std::cin, products);
                    std::cout << "Customers CSV path: ";
                    std::getline(std::cin, customers);

                    store = DataLoader::loadAll(order_items, orders, products, customers);
                    std::cout << "[Data loaded successfully]\n";
                    break;
                }

                //Path to show stats
                case 2: {
                    if (store.orders.empty()) {
                        std::cout << "No data loaded yet. Please load data first.\n";
                        break;
                    }
                    auto stats = dataStats::compute(store);
                    dataStats::print(stats, store);
                    break;
                }

                //Path for benchmark merge sort vs quick sort
                case 3: {
                    if (store.orders.empty()) {
                        std::cout << "Load data first.\n";
                        break;
                    }

                    std::vector<int> arr;
                    arr.reserve(10000);
                    for (int i = 0; i < 10000; ++i)
                        arr.push_back(rand() % 10000);

                    auto comp = [](int a, int b) { return a < b; };

                    long long merge_ms = Sorter::benchmark<int, decltype(comp)>(
                        arr, comp, [](std::vector<int>& v, const decltype(comp)& c){ Sorter::mergeSort(v, c); }
                    );
                    long long quick_ms = Sorter::benchmark<int, decltype(comp)>(
                        arr, comp, [](std::vector<int>& v, const decltype(comp)& c){ Sorter::quickSort(v, c); }
                    );

                    std::cout << "Merge Sort: " << merge_ms << " ms\n";
                    std::cout << "Quick Sort: " << quick_ms << " ms\n";
                    break;
                }
                
                //Build Graph
                case 4: {
                    if (store.orders.empty()) {
                        std::cout << "No data loaded yet. Choose option 1 first.\n";
                        break;
                    }

                    //Group products by orderId
                    std::unordered_map<std::string, std::vector<std::string>> orderToProducts;
                    orderToProducts.reserve(store.orders.size());

                    for (const auto& o : store.orders) {
                        auto& vec = orderToProducts[o.orderId];
                        if (vec.empty() || std::find(vec.begin(), vec.end(), o.productId) == vec.end())
                            vec.push_back(o.productId);
                    }

                    //Add edges between every unordered pair in an order
                    std::size_t pairs_added = 0;
                    for (const auto& kv : orderToProducts) {
                        const auto& prods = kv.second;
                        for (std::size_t i = 0; i < prods.size(); ++i) {
                            for (std::size_t j = i + 1; j < prods.size(); ++j) {
                                graph_.addEdge(prods[i], prods[j]);
                                ++pairs_added;
                            }
                        }
                    }
                    graphBuilt_ = true;
                    std::cout << "[Graph built]\n";
                    std::cout << "  Orders grouped: " << orderToProducts.size() << "\n";
                    std::cout << "  Nodes:          " << graph_.nodeCount() << "\n";
                    std::cout << "  Unique edges:   " << graph_.edgeCount() << "\n";
                    break;
                }

                //Query graph
                case 5: {
                    if (!graphBuilt_) {
                        std::cout << "Graph not built yet. Choose option 4 first.\n";
                        break;
                    }
                    std::string pid;
                    std::cout << "Enter productId to get recommendations: ";
                    std::getline(std::cin, pid);
                    if (pid.empty()) {
                        std::cout << "No productId given.\n";
                        break;
                    }

                    int limit = 5;
                    std::cout << "How many suggestions? [default 5]: ";
                    std::string lim;
                    std::getline(std::cin, lim);
                    try { if (!lim.empty()) limit = std::stoi(lim); } catch (...) {}
                    auto recs = graph_.suggest(pid, limit);
                    if (recs.empty()) {
                        std::cout << "No suggestions found for that product.\n";
                        break;
                    }

                    std::cout << "Recommendations for " << pid << ":\n";
                    for (const auto& r : recs) {
                        auto it = store.products.find(r);
                        if (it != store.products.end() && !it->second.title.empty())
                            std::cout << "  - " << r << "  [" << it->second.title << "]\n";
                        else
                            std::cout << "  - " << r << "\n";
                    }
                    break;
                }

                default:
                    std::cout << "Invalid option.\n";
                    break;
            }
        }
    }
}