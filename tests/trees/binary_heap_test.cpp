#include "dsa/trees/binary_heap.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using dsa::BinaryHeap;

// Vacía un montículo con extract_top() repetido y devuelve la secuencia
// resultante. Para un max-heap debe salir no creciente; para un min-heap,
// no decreciente. También destruye el montículo en el proceso, lo cual
// está bien porque cada prueba construye su propia instancia.
template <typename T, typename Compare>
std::vector<T> drain(BinaryHeap<T, Compare>& heap) {
    std::vector<T> out;
    out.reserve(heap.size());
    while (!heap.empty()) {
        out.push_back(heap.extract_top());
    }
    return out;
}

// ---------------------------------------------------------------------
// Construcción / estado vacío
// ---------------------------------------------------------------------

TEST(BinaryHeapConstruction, DefaultConstructedIsEmpty) {
    BinaryHeap<int> heap;
    EXPECT_TRUE(heap.empty());
    EXPECT_EQ(heap.size(), 0u);
}

TEST(BinaryHeapConstruction, TopOnEmptyHeapThrows) {
    BinaryHeap<int> heap;
    EXPECT_THROW(heap.top(), std::out_of_range);
}

TEST(BinaryHeapConstruction, ExtractTopOnEmptyHeapThrows) {
    BinaryHeap<int> heap;
    EXPECT_THROW(heap.extract_top(), std::out_of_range);
}

TEST(BinaryHeapConstruction, SingleElement) {
    BinaryHeap<int> heap;
    heap.insert(42);
    EXPECT_FALSE(heap.empty());
    EXPECT_EQ(heap.size(), 1u);
    EXPECT_EQ(heap.top(), 42);
    EXPECT_EQ(heap.extract_top(), 42);
    EXPECT_TRUE(heap.empty());
}

// ---------------------------------------------------------------------
// Orden de max-heap (Compare por defecto = std::less<T>)
// ---------------------------------------------------------------------

TEST(BinaryHeapMaxHeap, InsertInIncreasingOrderExtractsDescending) {
    BinaryHeap<int> heap;
    for (int i = 0; i < 10; ++i) {
        heap.insert(i);
    }
    EXPECT_EQ(drain(heap), (std::vector<int>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
}

TEST(BinaryHeapMaxHeap, InsertInDecreasingOrderExtractsDescending) {
    BinaryHeap<int> heap;
    for (int i = 9; i >= 0; --i) {
        heap.insert(i);
    }
    EXPECT_EQ(drain(heap), (std::vector<int>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
}

TEST(BinaryHeapMaxHeap, InsertInArbitraryOrderExtractsDescending) {
    BinaryHeap<int> heap;
    for (int value : {5, 1, 9, 3, 7, 2, 8, 0, 6, 4}) {
        heap.insert(value);
    }
    EXPECT_EQ(drain(heap), (std::vector<int>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
}

TEST(BinaryHeapMaxHeap, TopReflectsCurrentMaximumAfterEachInsert) {
    BinaryHeap<int> heap;
    heap.insert(5);
    EXPECT_EQ(heap.top(), 5);
    heap.insert(2);
    EXPECT_EQ(heap.top(), 5);
    heap.insert(9);
    EXPECT_EQ(heap.top(), 9);
    heap.insert(9);
    EXPECT_EQ(heap.top(), 9);
}

TEST(BinaryHeapMaxHeap, DuplicatesAreHandledCorrectly) {
    BinaryHeap<int> heap;
    for (int value : {3, 3, 1, 1, 2, 2, 3, 1}) {
        heap.insert(value);
    }
    EXPECT_EQ(drain(heap), (std::vector<int>{3, 3, 3, 2, 2, 1, 1, 1}));
}

TEST(BinaryHeapMaxHeap, ManyElementsMaintainHeapPropertyThroughoutMixedOps) {
    BinaryHeap<int> heap;
    std::vector<int> values;
    for (int i = 0; i < 200; ++i) {
        int value = (i * 37) % 101;
        values.push_back(value);
        heap.insert(value);
    }
    // Intercala algunas extracciones para ejercitar sift_down a mitad de
    // camino, luego vacía el resto; el multiconjunto total de elementos
    // extraídos debe seguir siendo exactamente la entrada ordenada.
    std::vector<int> extracted;
    for (int i = 0; i < 50; ++i) {
        extracted.push_back(heap.extract_top());
    }
    auto rest = drain(heap);
    extracted.insert(extracted.end(), rest.begin(), rest.end());

    std::vector<int> expected = values;
    std::sort(expected.begin(), expected.end(), std::greater<int>());
    EXPECT_EQ(extracted, expected);
}

// ---------------------------------------------------------------------
// Orden de min-heap (Compare = std::greater<T>)
// ---------------------------------------------------------------------

TEST(BinaryHeapMinHeap, InsertInArbitraryOrderExtractsAscending) {
    BinaryHeap<int, std::greater<int>> heap;
    for (int value : {5, 1, 9, 3, 7, 2, 8, 0, 6, 4}) {
        heap.insert(value);
    }
    EXPECT_EQ(drain(heap), (std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST(BinaryHeapMinHeap, TopReflectsCurrentMinimum) {
    BinaryHeap<int, std::greater<int>> heap;
    heap.insert(5);
    EXPECT_EQ(heap.top(), 5);
    heap.insert(9);
    EXPECT_EQ(heap.top(), 5);
    heap.insert(1);
    EXPECT_EQ(heap.top(), 1);
}

// ---------------------------------------------------------------------
// build_heap mediante el constructor de rango de iteradores
// ---------------------------------------------------------------------

TEST(BinaryHeapBuildHeap, FromUnsortedVectorMaxHeap) {
    std::vector<int> values{4, 10, 3, 5, 1, 8, 2, 9, 7, 6};
    BinaryHeap<int> heap(values.begin(), values.end());
    EXPECT_EQ(heap.size(), values.size());
    EXPECT_EQ(drain(heap), (std::vector<int>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
}

TEST(BinaryHeapBuildHeap, FromUnsortedVectorMinHeap) {
    std::vector<int> values{4, 10, 3, 5, 1, 8, 2, 9, 7, 6};
    BinaryHeap<int, std::greater<int>> heap(values.begin(), values.end());
    EXPECT_EQ(drain(heap), (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
}

TEST(BinaryHeapBuildHeap, FromEmptyRangeIsEmpty) {
    std::vector<int> values;
    BinaryHeap<int> heap(values.begin(), values.end());
    EXPECT_TRUE(heap.empty());
}

TEST(BinaryHeapBuildHeap, FromSingleElementRange) {
    std::vector<int> values{7};
    BinaryHeap<int> heap(values.begin(), values.end());
    EXPECT_EQ(heap.size(), 1u);
    EXPECT_EQ(heap.top(), 7);
}

TEST(BinaryHeapBuildHeap, WithDuplicates) {
    std::vector<int> values{4, 4, 4, 1, 1, 9, 9, 2};
    BinaryHeap<int> heap(values.begin(), values.end());
    std::vector<int> expected = values;
    std::sort(expected.begin(), expected.end(), std::greater<int>());
    EXPECT_EQ(drain(heap), expected);
}

// ---------------------------------------------------------------------
// clear()
// ---------------------------------------------------------------------

TEST(BinaryHeapClear, ResetsSizeButKeepsCapacity) {
    BinaryHeap<int> heap;
    for (int i = 0; i < 10; ++i) {
        heap.insert(i);
    }
    std::size_t capacity_before = heap.capacity();
    heap.clear();
    EXPECT_TRUE(heap.empty());
    EXPECT_EQ(heap.size(), 0u);
    EXPECT_EQ(heap.capacity(), capacity_before);
}

TEST(BinaryHeapClear, HeapIsUsableAfterClear) {
    BinaryHeap<int> heap;
    heap.insert(1);
    heap.insert(2);
    heap.clear();
    heap.insert(5);
    heap.insert(3);
    EXPECT_EQ(drain(heap), (std::vector<int>{5, 3}));
}

// ---------------------------------------------------------------------
// Regla de los cinco: copia / movimiento
// ---------------------------------------------------------------------

TEST(BinaryHeapRuleOfFive, CopyConstructorIsDeep) {
    BinaryHeap<int> original;
    for (int value : {3, 1, 4, 1, 5, 9, 2, 6}) {
        original.insert(value);
    }
    BinaryHeap<int> copy(original);
    copy.insert(100);
    EXPECT_NE(original.size(), copy.size());
    EXPECT_EQ(copy.top(), 100);
    EXPECT_EQ(original.top(), 9);
}

TEST(BinaryHeapRuleOfFive, CopyAssignment) {
    BinaryHeap<int> original;
    original.insert(1);
    original.insert(2);
    BinaryHeap<int> other;
    other.insert(99);
    other = original;
    EXPECT_EQ(other.size(), original.size());
    EXPECT_EQ(other.top(), 2);
}

TEST(BinaryHeapRuleOfFive, MoveConstructorStealsBuffer) {
    BinaryHeap<int> original;
    original.insert(1);
    original.insert(2);
    original.insert(3);
    BinaryHeap<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_EQ(moved.top(), 3);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(BinaryHeapRuleOfFive, MoveAssignment) {
    BinaryHeap<int> original;
    original.insert(1);
    original.insert(2);
    BinaryHeap<int> other;
    other.insert(99);
    other = std::move(original);
    EXPECT_EQ(other.size(), 2u);
    EXPECT_EQ(other.top(), 2);
}

TEST(BinaryHeapRuleOfFive, SwapExchangesContents) {
    BinaryHeap<int> a;
    a.insert(1);
    a.insert(2);
    BinaryHeap<int> b;
    b.insert(10);
    b.insert(20);
    b.insert(30);
    a.swap(b);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a.top(), 30);
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.top(), 2);
}

// ---------------------------------------------------------------------
// Tipo de elemento no trivial (std::string)
// ---------------------------------------------------------------------

TEST(BinaryHeapStringType, OrdersLexicographically) {
    BinaryHeap<std::string> heap;
    for (const std::string& value : {std::string("banana"), std::string("apple"),
                                     std::string("cherry"), std::string("date")}) {
        heap.insert(value);
    }
    EXPECT_EQ(heap.top(), "date");
}

}  // namespace
