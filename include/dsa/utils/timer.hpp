#ifndef DSA_UTILS_TIMER_HPP
#define DSA_UTILS_TIMER_HPP

#include <chrono>
#include <iostream>
#include <string>

namespace dsa {

/**
 * @brief Timer RAII simple para benchmarking.
 * Imprime la duración por stdout al destruirse.
 */
class Timer {
   public:
    explicit Timer(const std::string& name = "Task")
        : name_(name), start_(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "[" << name_ << "] Duración: " << duration << " microsegundos\n";
    }

    // Evita la copia
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

   private:
    std::string name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

}  // namespace dsa

#endif  // DSA_UTILS_TIMER_HPP
