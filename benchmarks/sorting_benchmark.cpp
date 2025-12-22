#include <iostream>
#include <vector>
#include <algorithm>
#include "dsa/utils/timer.hpp"

int main() {
    dsa::Timer t("SortingBenchmark");
    std::vector<int> v = {5, 2, 9, 1, 5, 6};
    std::sort(v.begin(), v.end());
    return 0;
}
