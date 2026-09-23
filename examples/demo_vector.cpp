#include <iostream>
#include <vector>

#include "dsa/utils/timer.hpp"

int main() {
    dsa::Timer t("VectorDemo");
    std::vector<int> v;
    v.push_back(1);
    std::cout << "Vector size: " << v.size() << std::endl;
    return 0;
}
