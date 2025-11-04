#pragma once
#include "DataLoader.h"

namespace ecomm {
    struct basicStats {
        //Initialize all to 0
        int orderCount = 0;
        //Distinct, no duplicates
        int distinctCustomers = 0;
        int distinctProducts  = 0;
        //Dollar amount so double
        double totalRev = 0.0;
        double avgVal = 0.0;
    };
    //Classs to print stats
    class dataStats {
    public:
        //Calculate basic stats from stored data and prints
        static basicStats compute(const dataStore& data);
        static void print(const basicStats& stats, const dataStore& data);
    };
}
