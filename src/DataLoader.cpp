#include "DataLoader.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ecomm {

//Deals with the whitespaces
    static std::string trim(std::string s){
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c){ return !std::isspace(c); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), s.end());
        return s;
    }

//Deal with the csv file
    static std::vector<std::string> splitRow(const std::string& line, char delim=','){
        std::vector<std::string> out; out.reserve(16);
        std::string current; current.reserve(line.size());
        bool inq=false;
        for(size_t i=0;i<line.size();++i){
            char c = line[i];
            if(inq){
                if(c == '"' && i + 1 < line.size() && line[i+1] == '"'){ current.push_back('"'); ++i; }
                else if(c == '"'){ inq = false; }
                else current.push_back(c);
            }else{
                if(c == '"') inq=true;
                else if(c == delim){ out.push_back(trim(current)); current.clear(); }
                else current.push_back(c);
            }
        }
        out.push_back(trim(current));
        return out;
    }

//Convert a money string into int cents
    static std::optional<long long> moneyToCents(std::string s){
        s = trim(s);
        if (s.empty()) return std::nullopt;
        s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c){
            return std::isspace(c) || c=='$' || c == ',';
        }), s.end());
        char* end = nullptr;
        double v = std::strtod(s.c_str(), &end);
        if(end == s.c_str() || *end != '\0') return std::nullopt;
        return static_cast<long long>(std::llround(v*100.0));
    }

//LOADERS vv

// Build map from olist_orders_dataset.csv
    static std::unordered_map<std::string,std::string>
    loadOrderToCustomerMap(const std::string& ordersCsv, char delim = ',', bool has_header = true){
        std::unordered_map<std::string,std::string> m;
        std::ifstream in(ordersCsv);
        if(!in){ std::cerr << "[DataLoader] Could not open: " << ordersCsv << "\n"; return m; }
        std::string line;
        if(has_header && std::getline(in,line)) {}
        while(std::getline(in,line)){
            if(line.empty()) continue;
            auto col = splitRow(line, delim);
            if(col.size() >= 2){
                const std::string& orderId = col[0];
                const std::string& customerId = col[1];
                if(!orderId.empty() && !customerId.empty()) m[orderId] = customerId;
            }
        }
        std::cout << "[DataLoader] order->customer map: " << m.size() << "\n";
        return m;
    }

// Load order_items and join to get customerId
    static std::vector<Order>
    loadOrderItemsJoined(const std::string& orderItemsCsv, const std::unordered_map<std::string,std::string>& orderToCustomer, char delim = ',', bool has_header = true){
        std::vector<Order> rows;
        std::ifstream in(orderItemsCsv);
        if(!in){ std::cerr << "[DataLoader] Could not open: " << orderItemsCsv << "\n"; return rows; }
        std::string line;
        if(has_header && std::getline(in,line)) {}
        std::size_t bad = 0;

        while(std::getline(in,line)){
            if(line.empty()) continue;
            auto col = splitRow(line, delim);
            if(col.size() < 6){ ++bad; continue; }
            auto cents = moneyToCents(col[5]);
            if(!cents){ ++bad; continue; }
            Order o;
            o.orderId = col[0];
            o.productId = col[2];
            o.quantity = 1;
            o.price_cents = static_cast<int64_t>(*cents);
            auto it = orderToCustomer.find(o.orderId);
            if(it != orderToCustomer.end()) o.customerId = it->second;
            rows.push_back(std::move(o));
        }

        std::cout << "[DataLoader] Order items (joined): " << rows.size()
                  << (bad ? (" (" + std::to_string(bad) + " bad rows)") : "") << "\n";
        return rows;
    }

//Load products into a map of product_id
    static std::unordered_map<std::string, Product>
    loadProducts(const std::string& path, char delim=',', bool has_header=true){
        std::unordered_map<std::string, Product> map;
        std::ifstream in(path);
        if(!in){ std::cerr << "[DataLoader] Could not open: " << path << "\n"; return map; }
        std::string line;
        if(has_header && std::getline(in,line)) {}
        while(std::getline(in,line)){
            if(line.empty()) continue;
            auto col = splitRow(line, delim);
            if(col.size() < 1) continue;
            Product prod{};
            prod.productId = col[0];
            if(col.size()>1) prod.title = col[1];
            if(col.size()>1) prod.category = col[1];
            if(!prod.productId.empty()) map[prod.productId] = std::move(prod);
        }
        std::cout << "[DataLoader] Products: " << map.size() << "\n";
        return map;
    }

//Load customers into a map of customer_id
    static std::unordered_map<std::string, Customer>
    loadCustomers(const std::string& path, char delim=',', bool has_header=true){
        std::unordered_map<std::string, Customer> map;
        std::ifstream in(path);
        if(!in){ std::cerr << "[DataLoader] Could not open: " << path << "\n"; return map; }
        std::string line;
        if(has_header && std::getline(in,line)) {}
        while(std::getline(in,line)){
            if(line.empty()) continue;
            auto col = splitRow(line, delim);
            if(col.size() < 1) continue;
            Customer c{};
            c.customerId = col[0];
            if(col.size()>3) c.name = col[3];
            if(col.size()>4) c.region = col[4];
            if(!c.customerId.empty()) map[c.customerId] = std::move(c);
        }
        std::cout << "[DataLoader] Customers: " << map.size() << "\n";
        return map;
    }

//Load the full dataset
    DataStore DataLoader::loadAll(const std::string& orderItemsFile, const std::string& ordersFile, const std::string& productsFile, const std::string& customersFile) {
        DataStore stored;
        auto orderToCustomer = loadOrderToCustomerMap(ordersFile);
        stored.orders    = loadOrderItemsJoined(orderItemsFile, orderToCustomer);
        stored.products  = loadProducts(productsFile);
        stored.customers = loadCustomers(customersFile);
        return stored;
    }
}
