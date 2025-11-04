cmake_minimum_required(VERSION 3.15)
project(ECommerceDSA CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ecomm_lib
        src/DataLoader.cpp
        src/DataStats.cpp
        src/Graph.cpp
        src/MainApp.cpp
)

target_include_directories(ecomm_lib PUBLIC include)

add_executable(ECommerceDSA src/main.cpp)
target_link_libraries(ECommerceDSA PRIVATE ecomm_lib)