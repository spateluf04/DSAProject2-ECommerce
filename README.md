# E-Commerce DSA: Product Recommendations from Real Order Data

A C++17 command-line app that loads about 100,000 real e-commerce orders, computes sales statistics, benchmarks two sorting algorithms, and recommends products using a **co-purchase graph**: products that are often bought alongside the same items get recommended together.

Built as Project 2 for COP3530 (Data Structures & Algorithms) at the University of Florida.

## Features

| Menu option | What it does | Data structures / algorithms |
|---|---|---|
| **1. Load Data** | Parses four Olist CSVs and joins order items to customers | Custom quote-aware CSV parser, hash maps (`unordered_map`) |
| **2. Show Statistics** | Line-item count, unique customers and products, total revenue, average item price | Hash sets (`unordered_set`) for distinct counts |
| **3. Sort Benchmark** | Times hand-written Merge Sort against Quick Sort | Generic templated sorts in [`Sort.h`](include/Sort.h) |
| **4. Build Graph** | Links every pair of products that appear in the same order | Undirected graph as an adjacency list |
| **5. Query Graph** | Recommends products for a given product ID | Two-hop neighbour traversal with frequency ranking |

## How the recommendations work

1. **Build:** order items are grouped by `order_id`. Each pair of distinct products in one order gets an undirected edge (`Graph::addEdge`). Edges are stored as `unordered_map<string, unordered_set<string>>`, so repeated pairs never become duplicate edges.
2. **Query:** for product **P**, `Graph::suggest` visits each neighbour of P and then each of *their* neighbours. It counts how often every product turns up on those two-hop paths. The products with the highest counts are recommended first.

In short: *"customers who bought what goes with P also bought these."* A product's score is the number of P's co-purchase partners it shares.

## Getting started

### Requirements
- A C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.15 or newer

### Build

```bash
git clone https://github.com/spateluf04/DSAProject2-ECommerce.git
cd DSAProject2-ECommerce
cmake -S . -B build
cmake --build build
```

The executable is written to `build/ECommerceDSA`, or `build\Debug\ECommerceDSA.exe` with Visual Studio. In CLion, open the folder and run the `ECommerceDSA` target.

### Data

The CSVs are already in [`data/`](data/). Choose option **1** and enter these four paths when prompted. Paths are relative to the directory you run from; the ones below assume the repo root:

```
Order ITEMS CSV path: data/olist_order_items_dataset.csv
Orders CSV path:      data/olist_orders_dataset.csv
Products CSV path:    data/olist_products_dataset.csv
Customers CSV path:   data/olist_customers_dataset.csv
```

## Usage

```
==== E-COMMERCE DSA MENU ====
1. Load Data
2. Show Statistics
3. Sort Benchmark
4. Build Graph
5. Query Graph
0. Exit
>
```

Run option 1 first. Options 2–4 need loaded data, and option 5 needs the graph from option 4.

### Expected results on the included dataset

I computed these figures from the CSVs with the same rules the program uses. They are a reference for checking your build, not captured program output.

| | |
|---|---|
| Order line items | 112,650 |
| Orders | 99,441 |
| Unique customers (with items) | 98,666 |
| Unique products | 32,951 |
| Total revenue (item prices, excluding freight) | $13,591,643.70 |
| Average price per line item | $120.65 |
| Graph nodes / unique edges | 4,885 / 4,058 |

Most Olist orders contain a single product, so only about 4,900 of the roughly 33,000 products end up with any co-purchase edges. A product that was always bought alone gets no recommendations.

To try a query, use product `99a4788cb24856965c36a24e339b6058`, the most connected product in the graph (23 co-purchase partners).

## Project structure

```
├── CMakeLists.txt
├── include/
│   ├── DataLoader.h   # Order / Product / Customer records and the DataStore
│   ├── DataStats.h    # Summary statistics
│   ├── Graph.h        # Co-purchase graph (adjacency list)
│   ├── MainApp.h      # Interactive menu
│   └── Sort.h         # Templated merge sort, quick sort, and benchmark helper
├── src/
│   ├── DataLoader.cpp # CSV parsing, money-to-cents conversion, order→customer join
│   ├── DataStats.cpp
│   ├── Graph.cpp
│   ├── MainApp.cpp
│   └── main.cpp
└── data/              # Olist dataset (see Dataset & licence)
```

## Implementation notes

- **Money is stored as integer cents** (`int64_t price_cents`), which avoids floating-point drift when summing more than 100,000 prices.
- **CSV parsing** handles quoted fields and escaped quotes (`""`) and trims whitespace. Malformed item rows are skipped and counted.
- **Statistics are per line item.** Each row of `order_items` is one record with quantity 1, so the "Orders" figure in option 2 counts line items (112,650), not distinct orders (99,441).
- **Product labels:** the Olist products file has no product names, so recommendations are labelled with the product's category (for example, `cama_mesa_banho`, which means bed/bath/table).
- **The sort benchmark** times both algorithms on the same 10,000 random integers, to compare them on identical input.

## Team

- [@spateluf04](https://github.com/spateluf04)
- [@Dmehta06](https://github.com/Dmehta06)
- [@Kush321](https://github.com/Kush321)

## Dataset & licence

The data is the [Brazilian E-Commerce Public Dataset by Olist](https://www.kaggle.com/datasets/olistbr/brazilian-ecommerce), released under [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/). Olist anonymised it before release: customers and sellers appear only as hashed IDs and zip-code prefixes. The included CSVs are redistributed unmodified under the same licence.
