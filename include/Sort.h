#pragma once
#include <vector>
#include <chrono>
#include <utility>
#include <algorithm>

namespace Sorter {

    //Merge two sorted halves
    template <typename T, typename Comp>
    static void mergeInto(std::vector<T>& a, std::vector<T>& tmp, size_t l, size_t m, size_t r, Comp comp) {
        size_t i=l, j=m, k=l;
        while (i<m && j<r) {
            if (comp(a[j], a[i])) tmp[k++] = std::move(a[j++]);
            else tmp[k++] = std::move(a[i++]);
        }
        while (i<m) tmp[k++] = std::move(a[i++]);
        while (j<r) tmp[k++] = std::move(a[j++]);
        for (size_t t=l; t<r; ++t) a[t] = std::move(tmp[t]);
    }

    //Recursive helper function
    template <typename T, typename Comp>
    static void msort(std::vector<T>& a, std::vector<T>& tmp, size_t l, size_t r, Comp comp) {
        if (r - l <= 1) return;
        size_t m = l + (r - l) / 2;
        msort(a, tmp, l, m, comp);
        msort(a, tmp, m, r, comp);
        mergeInto(a, tmp, l, m, r, comp);
    }

    //Merge sort function
    template <typename T, typename Comp>
    void mergeSort(std::vector<T>& a, Comp comp) {
        if (a.size() <= 1) return;
        std::vector<T> tmp(a.size());
        msort(a, tmp, 0, a.size(), comp);
    }

    //Recursive quicksort
    template <typename T, typename Comp>
    static void qsort(std::vector<T>& a, size_t l, size_t r, Comp comp) {
        if (r <= l) return;
        T pivot = a[l + (r - l) / 2];

        size_t i = l, j = r;
        while (i <= j) {
            while (comp(a[i], pivot)) ++i;
            while (comp(pivot, a[j])) {
                if (j == 0) break;
                --j;
            }
            if (i <= j) {
                std::swap(a[i], a[j]);
                ++i;
                if (j == 0) break;
                --j;
            }
        }
        if (j > l) qsort(a, l, j, comp);
        if (i < r) qsort(a, i, r, comp);
    }

    //Quicksort function
    template <typename T, typename Comp>
    void quickSort(std::vector<T>& a, Comp comp) {
        if (a.empty()) return;
        qsort(a, 0, a.size() - 1, comp);
    }

    //Benchmark function
    template <typename T, typename Comp, typename Algo>
    long long benchmark(std::vector<T> arr, Comp comp, Algo algo) {
        auto start = std::chrono::high_resolution_clock::now();
        algo(arr, comp);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
}