#include "DataStats.h"
#include <iostream>
#include <unordered_set>

namespace ecomm {
    //take dataset in and return basic stat
    basicStats dataStats::compute(const dataStore& data) {
        basicStats s{};
        //set order count to number of order entries in data.orders
        s.orderCount = data.orders.size();


        //DATA STRUCTURE: 2 hash sets to collect customer and product ids
        std::unordered_set<std::string> cstmr, prdct;
        double revenue = 0.0;

        //iterate to num of orders - 1
        for (int i = 0; i < data.orders.size(); ++i) {
            //ref to order object at i
            const Order& o = data.orders[i];
            cstmr.insert(o.customer_id);
            prdct.insert(o.product_id);
            //convert cents to dollars
            double price_dollars = static_cast<double>(o.price_cents) / 100.0;
            //adds order contribution to total revenue
            revenue += price_dollars * static_cast<double>(o.quantity);
        }


        s.distinctCustomers = cstmr.size();
        s.distinctProducts = prdct.size();
        s.totalRev = revenue;
        //checks for at least one order before dividing or sets to 0
        if (s.orderCount != 0) {
            s.avgVal = revenue / static_cast<double>(s.orderCount);
        } else {
            s.avgVal = 0.0;
        }
        return s;
    }

    //prints menu for basic stats if option 2 selected
    void dataStats::print(const basicStats& s, const dataStore&) {
        std::cout << "==== BASIC STATS ====\n";
        std::cout << "Orders:          " << s.orderCount << "\n";
        std::cout << "Unique customers:" << s.distinctCustomers << "\n";
        std::cout << "Unique products: " << s.distinctProducts << "\n";
        std::cout << "Total revenue:   $" << s.totalRev << "\n";
        std::cout << "Avg order value: $" << s.avgVal << "\n";
    }

}
