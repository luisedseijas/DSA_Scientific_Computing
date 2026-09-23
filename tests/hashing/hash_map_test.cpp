#include "dsa/hashing/hash_map.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <string>

namespace {

using dsa::HashMap;

// Una función de hash deliberadamente débil, usada para forzar colisiones y
// ejercitar el camino de encadenamiento separado sin importar bucket_count.
struct BadHash {
    std::size_t operator()(int x) const { return static_cast<std::size_t>(x % 4); }
};

// ---------------------------------------------------------------------
// Construcción
// ---------------------------------------------------------------------

TEST(HashMapConstruction, DefaultConstructedIsEmpty) {
    HashMap<std::string, int> map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0u);
    EXPECT_EQ(map.load_factor(), 0.0);
}

// ---------------------------------------------------------------------
// insert / find / contains (insertar / buscar / contiene)
// ---------------------------------------------------------------------

TEST(HashMapBasics, InsertAndFindString) {
    HashMap<std::string, int> map;
    EXPECT_TRUE(map.insert("one", 1));
    EXPECT_TRUE(map.insert("two", 2));

    ASSERT_NE(map.find("one"), nullptr);
    EXPECT_EQ(*map.find("one"), 1);
    ASSERT_NE(map.find("two"), nullptr);
    EXPECT_EQ(*map.find("two"), 2);
    EXPECT_EQ(map.find("three"), nullptr);
}

TEST(HashMapBasics, InsertAndFindInt) {
    HashMap<int, std::string> map;
    map.insert(1, "a");
    map.insert(2, "b");
    ASSERT_NE(map.find(1), nullptr);
    EXPECT_EQ(*map.find(1), "a");
}

TEST(HashMapBasics, ContainsReflectsPresence) {
    HashMap<std::string, int> map;
    EXPECT_FALSE(map.contains("x"));
    map.insert("x", 10);
    EXPECT_TRUE(map.contains("x"));
}

TEST(HashMapBasics, GetReturnsOptional) {
    HashMap<std::string, int> map;
    EXPECT_FALSE(map.get("missing").has_value());
    map.insert("present", 42);
    ASSERT_TRUE(map.get("present").has_value());
    EXPECT_EQ(*map.get("present"), 42);
}

TEST(HashMapBasics, SizeTracksInsertions) {
    HashMap<int, int> map;
    for (int i = 0; i < 10; ++i) map.insert(i, i * i);
    EXPECT_EQ(map.size(), 10u);
}

// ---------------------------------------------------------------------
// Claves duplicadas
// ---------------------------------------------------------------------

TEST(HashMapDuplicates, InsertOverwritesExistingKey) {
    HashMap<std::string, int> map;
    EXPECT_TRUE(map.insert("k", 1));
    EXPECT_FALSE(map.insert("k", 2));  // la segunda inserción sobrescribe, no es nueva
    ASSERT_NE(map.find("k"), nullptr);
    EXPECT_EQ(*map.find("k"), 2);
    EXPECT_EQ(map.size(), 1u);
}

// ---------------------------------------------------------------------
// operator[] (operador de indexación)
// ---------------------------------------------------------------------

TEST(HashMapIndexOperator, CreatesEntryWithDefaultValueIfMissing) {
    HashMap<std::string, int> map;
    EXPECT_EQ(map["new_key"], 0);  // int{} == 0
    EXPECT_TRUE(map.contains("new_key"));
    EXPECT_EQ(map.size(), 1u);
}

TEST(HashMapIndexOperator, ReturnsReferenceThatCanBeMutated) {
    HashMap<std::string, int> map;
    map["counter"]++;
    map["counter"]++;
    map["counter"] += 3;
    EXPECT_EQ(map["counter"], 5);
    EXPECT_EQ(map.size(), 1u);
}

TEST(HashMapIndexOperator, DoesNotOverwriteExistingValue) {
    HashMap<std::string, int> map;
    map.insert("k", 7);
    EXPECT_EQ(map["k"], 7);
}

// ---------------------------------------------------------------------
// erase (eliminar)
// ---------------------------------------------------------------------

TEST(HashMapErase, RemovesExistingKey) {
    HashMap<std::string, int> map;
    map.insert("a", 1);
    map.insert("b", 2);
    EXPECT_TRUE(map.erase("a"));
    EXPECT_FALSE(map.contains("a"));
    EXPECT_TRUE(map.contains("b"));
    EXPECT_EQ(map.size(), 1u);
}

TEST(HashMapErase, ReturnsFalseForMissingKey) {
    HashMap<std::string, int> map;
    map.insert("a", 1);
    EXPECT_FALSE(map.erase("nonexistent"));
    EXPECT_EQ(map.size(), 1u);
}

// ---------------------------------------------------------------------
// clear (vaciar)
// ---------------------------------------------------------------------

TEST(HashMapClear, RemovesAllEntries) {
    HashMap<int, int> map;
    for (int i = 0; i < 20; ++i) map.insert(i, i);
    map.clear();
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0u);
    for (int i = 0; i < 20; ++i) EXPECT_FALSE(map.contains(i));
}

// ---------------------------------------------------------------------
// Colisiones forzadas mediante una función de hash trivial
// ---------------------------------------------------------------------

TEST(HashMapCollisions, AllKeysAccessibleDespiteForcedCollisions) {
    HashMap<int, int, BadHash> map(4);  // BadHash % 4 -> solo 4 ranuras distintas
    for (int i = 0; i < 50; ++i) {
        map.insert(i, i * 10);
    }
    EXPECT_EQ(map.size(), 50u);
    for (int i = 0; i < 50; ++i) {
        ASSERT_NE(map.find(i), nullptr) << "falta la clave " << i;
        EXPECT_EQ(*map.find(i), i * 10);
    }
}

TEST(HashMapCollisions, EraseWorksWithinCollidingBucket) {
    HashMap<int, int, BadHash> map(4);
    map.insert(1, 100);  // bucket 1
    map.insert(5, 500);  // bucket 1 (5 % 4 == 1)
    map.insert(9, 900);  // bucket 1 (9 % 4 == 1)

    EXPECT_TRUE(map.erase(5));
    EXPECT_TRUE(map.contains(1));
    EXPECT_FALSE(map.contains(5));
    EXPECT_TRUE(map.contains(9));
}

// ---------------------------------------------------------------------
// Rehash automático
// ---------------------------------------------------------------------

TEST(HashMapRehash, BucketCountGrowsAndAllElementsRemainAccessible) {
    HashMap<int, int> map(4);
    const std::size_t initial_buckets = map.bucket_count();

    const int kCount = 1000;
    for (int i = 0; i < kCount; ++i) {
        map.insert(i, i * 2);
    }

    EXPECT_GT(map.bucket_count(), initial_buckets);
    EXPECT_LE(map.load_factor(), (HashMap<int, int>::kMaxLoadFactor) + 1e-9);
    EXPECT_EQ(map.size(), static_cast<std::size_t>(kCount));

    for (int i = 0; i < kCount; ++i) {
        ASSERT_NE(map.find(i), nullptr) << "falta la clave " << i << " tras el rehash";
        EXPECT_EQ(*map.find(i), i * 2);
    }
}

TEST(HashMapRehash, RehashAlsoOccursWithForcedCollisionHash) {
    HashMap<int, int, BadHash> map(2);
    for (int i = 0; i < 200; ++i) map.insert(i, i);
    EXPECT_EQ(map.size(), 200u);
    for (int i = 0; i < 200; ++i) {
        ASSERT_NE(map.find(i), nullptr);
    }
}

}  // namespace
