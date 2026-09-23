#include "dsa/hashing/hash_set.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <string>

namespace {

using dsa::HashSet;

// A deliberately weak hash function used to force collisions and exercise
// the separate-chaining path regardless of bucket_count.
struct BadHash {
    std::size_t operator()(int x) const { return static_cast<std::size_t>(x % 4); }
};

// ---------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------

TEST(HashSetConstruction, DefaultConstructedIsEmpty) {
    HashSet<std::string> set;
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0u);
    EXPECT_EQ(set.load_factor(), 0.0);
}

// ---------------------------------------------------------------------
// insert / find / contains
// ---------------------------------------------------------------------

TEST(HashSetBasics, InsertAndContainsString) {
    HashSet<std::string> set;
    EXPECT_TRUE(set.insert("apple"));
    EXPECT_TRUE(set.insert("banana"));
    EXPECT_TRUE(set.contains("apple"));
    EXPECT_TRUE(set.contains("banana"));
    EXPECT_FALSE(set.contains("cherry"));
}

TEST(HashSetBasics, InsertAndContainsInt) {
    HashSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    EXPECT_TRUE(set.contains(2));
    EXPECT_FALSE(set.contains(42));
}

TEST(HashSetBasics, FindReturnsPointerToStoredKey) {
    HashSet<std::string> set;
    set.insert("k");
    const std::string* found = set.find("k");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(*found, "k");
    EXPECT_EQ(set.find("missing"), nullptr);
}

TEST(HashSetBasics, SizeTracksInsertions) {
    HashSet<int> set;
    for (int i = 0; i < 10; ++i) set.insert(i);
    EXPECT_EQ(set.size(), 10u);
}

// ---------------------------------------------------------------------
// Duplicate keys
// ---------------------------------------------------------------------

TEST(HashSetDuplicates, InsertingSameKeyTwiceIsANoOp) {
    HashSet<std::string> set;
    EXPECT_TRUE(set.insert("dup"));
    EXPECT_FALSE(set.insert("dup"));  // already present
    EXPECT_EQ(set.size(), 1u);
}

// ---------------------------------------------------------------------
// erase
// ---------------------------------------------------------------------

TEST(HashSetErase, RemovesExistingKey) {
    HashSet<std::string> set;
    set.insert("a");
    set.insert("b");
    EXPECT_TRUE(set.erase("a"));
    EXPECT_FALSE(set.contains("a"));
    EXPECT_TRUE(set.contains("b"));
    EXPECT_EQ(set.size(), 1u);
}

TEST(HashSetErase, ReturnsFalseForMissingKey) {
    HashSet<std::string> set;
    set.insert("a");
    EXPECT_FALSE(set.erase("nonexistent"));
    EXPECT_EQ(set.size(), 1u);
}

// ---------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------

TEST(HashSetClear, RemovesAllElements) {
    HashSet<int> set;
    for (int i = 0; i < 20; ++i) set.insert(i);
    set.clear();
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0u);
    for (int i = 0; i < 20; ++i) EXPECT_FALSE(set.contains(i));
}

// ---------------------------------------------------------------------
// Forced collisions via a trivial hash function
// ---------------------------------------------------------------------

TEST(HashSetCollisions, AllKeysAccessibleDespiteForcedCollisions) {
    HashSet<int, BadHash> set(4);  // BadHash % 4 -> only 4 distinct slots
    for (int i = 0; i < 50; ++i) {
        set.insert(i);
    }
    EXPECT_EQ(set.size(), 50u);
    for (int i = 0; i < 50; ++i) {
        EXPECT_TRUE(set.contains(i)) << "missing key " << i;
    }
}

TEST(HashSetCollisions, EraseWorksWithinCollidingBucket) {
    HashSet<int, BadHash> set(4);
    set.insert(1);  // bucket 1
    set.insert(5);  // bucket 1 (5 % 4 == 1)
    set.insert(9);  // bucket 1 (9 % 4 == 1)

    EXPECT_TRUE(set.erase(5));
    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.contains(5));
    EXPECT_TRUE(set.contains(9));
}

// ---------------------------------------------------------------------
// Automatic rehashing
// ---------------------------------------------------------------------

TEST(HashSetRehash, BucketCountGrowsAndAllElementsRemainAccessible) {
    HashSet<int> set(4);
    const std::size_t initial_buckets = set.bucket_count();

    const int kCount = 1000;
    for (int i = 0; i < kCount; ++i) {
        set.insert(i);
    }

    EXPECT_GT(set.bucket_count(), initial_buckets);
    EXPECT_LE(set.load_factor(), HashSet<int>::kMaxLoadFactor + 1e-9);
    EXPECT_EQ(set.size(), static_cast<std::size_t>(kCount));

    for (int i = 0; i < kCount; ++i) {
        EXPECT_TRUE(set.contains(i)) << "missing key " << i << " after rehash";
    }
}

TEST(HashSetRehash, RehashAlsoOccursWithForcedCollisionHash) {
    HashSet<int, BadHash> set(2);
    for (int i = 0; i < 200; ++i) set.insert(i);
    EXPECT_EQ(set.size(), 200u);
    for (int i = 0; i < 200; ++i) {
        EXPECT_TRUE(set.contains(i));
    }
}

}  // namespace
