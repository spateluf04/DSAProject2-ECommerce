#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace ecomm {

    struct Order {
        std::string orderId;
        std::string customerId;
        std::string productId;
        int32_t quantity = 0;
        int64_t price_cents = 0;
    };

    struct Product {
        std::string productId;
        std::string title;
        std::string category;
    };

    struct Customer {
        std::string customerId;
        std::string name;
        std::string region;
    };

    struct DataStore {
        std::vector<Order> orders;
        std::unordered_map<std::string, Product> products;
        std::unordered_map<std::string, Customer> customers;
    };

    class DataLoader {
    public:
        static DataStore loadAll(const std::string& orderItemsFile, const std::string& ordersFile, const std::string& productsFile, const std::string& customersFile);
    };

}