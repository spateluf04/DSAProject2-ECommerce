#pragma once
#include "DataLoader.h"
#include "DataStats.h"
#include "Graph.h"
#include <string>

namespace ecomm {

    class MainApp {

    public:
        void run();

    private:
        DataStore store;
        Graph graph_;
        bool graphBuilt_ = false;
    };

}