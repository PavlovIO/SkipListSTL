#include "SkipList.h"
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <random>

class SkipListTest : public ::testing::Test {
protected:
    void SetUp() override {
        sl_int = new SkipList<int>();
        sl_string = new SkipList<std::string>();
    }

    void TearDown() override {
        delete sl_int;
        delete sl_string;
    }

    SkipList<int>* sl_int;
    SkipList<std::string>* sl_string;
};

// Basic Functionality Tests
TEST_F(SkipListTest, Initialization) {
    EXPECT_TRUE(sl_int->empty());
    EXPECT_EQ(sl_int->size(), 0);
    EXPECT_FALSE(sl_int->contains(42));
}

TEST_F(SkipListTest, SingleInsert) {
    auto [it, inserted] = sl_int->insert(42);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(*(it), 42); 
    EXPECT_EQ(sl_int->size(), 1);
    EXPECT_TRUE(sl_int->contains(42));
    EXPECT_TRUE(sl_int->validate());
}

TEST_F(SkipListTest, DuplicateInsert) {
    sl_int->insert(42);
    auto [it, inserted] = sl_int->insert(42);
    EXPECT_FALSE(inserted);
    EXPECT_EQ(*it, 42);
    EXPECT_EQ(sl_int->size(), 1);
}

// Multiple Insert Tests
TEST_F(SkipListTest, MultipleInsertAscending) {
    for (int i = 0; i < 100; ++i) {
        sl_int->insert(i);
    }
    EXPECT_EQ(sl_int->size(), 100);
    EXPECT_TRUE(sl_int->validate());
    
    int count = 0;
    for (auto it = sl_int->begin(); it != sl_int->end(); ++it) {
        EXPECT_EQ(*it, count++);
    }
}

TEST_F(SkipListTest, MultipleInsertRandom) {
    std::vector<int> nums(100);
    std::iota(nums.begin(), nums.end(), 0);
    std::shuffle(nums.begin(), nums.end(), std::mt19937{std::random_device{}()});

    for (int num : nums) {
        sl_int->insert(num);
    }
    EXPECT_EQ(sl_int->size(), 100);
    EXPECT_TRUE(sl_int->validate());
    
    int prev = -1;
    for (auto it = sl_int->begin(); it != sl_int->end(); ++it) {
        EXPECT_GT(*it, prev);
        prev = *it;
    }
}

// Erase Tests
TEST_F(SkipListTest, SingleErase) {
    sl_int->insert(42);
    bool erased = sl_int->erase(42);
    EXPECT_TRUE(erased);
    EXPECT_EQ(sl_int->size(), 0);
    EXPECT_FALSE(sl_int->contains(42));
    EXPECT_TRUE(sl_int->validate());
}

TEST_F(SkipListTest, EraseNonExistent) {
    sl_int->insert(42);
    bool erased = sl_int->erase(24);
    EXPECT_FALSE(erased);
    EXPECT_EQ(sl_int->size(), 1);
}

TEST_F(SkipListTest, MultipleErase) {
    for (int i = 0; i < 100; ++i) {
        sl_int->insert(i);
    }
    
    for (int i = 0; i < 100; i += 2) {
        EXPECT_TRUE(sl_int->erase(i));
    }
    
    EXPECT_EQ(sl_int->size(), 50);
    EXPECT_TRUE(sl_int->validate());
    
    for (int i = 1; i < 100; i += 2) {
        EXPECT_TRUE(sl_int->contains(i));
    }
}

// Iterator Tests
TEST_F(SkipListTest, IteratorTraversal) {
    std::vector<int> nums = {3, 1, 4, 1, 5, 9, 2, 6};
    for (int num : nums) {
        sl_int->insert(num);
    }
    
    std::sort(nums.begin(), nums.end());
    nums.erase(std::unique(nums.begin(), nums.end()), nums.end());
    
    auto it = sl_int->begin();
    for (size_t i = 0; i < nums.size(); ++i) {
        ASSERT_NE(it, sl_int->end());
        EXPECT_EQ(*it, nums[i]);
        ++it;
    }
    EXPECT_EQ(it, sl_int->end());
}

TEST_F(SkipListTest, ConstIterator) {
    sl_int->insert(42);
    const SkipList<int>& const_sl = *sl_int;
    auto it = const_sl.begin();
    EXPECT_EQ(*it, 42);
}

// Find Tests
TEST_F(SkipListTest, FindExisting) {
    sl_int->insert(42);
    auto it = sl_int->find(42);
    EXPECT_NE(it, sl_int->end());
    EXPECT_EQ(*it, 42);
}

TEST_F(SkipListTest, FindNonExistent) {
    sl_int->insert(42);
    auto it = sl_int->find(24);
    EXPECT_EQ(it, sl_int->end());
}

// Boundary Tests
TEST_F(SkipListTest, LowerBound) {
    for (int i = 0; i < 100; i += 2) {
        sl_int->insert(i);
    }
    
    auto lb = sl_int->lower_bound(35);
    EXPECT_NE(lb, sl_int->end());
    EXPECT_EQ(*lb, 36);
    
    lb = sl_int->lower_bound(100);
    EXPECT_EQ(lb, sl_int->end());
}

TEST_F(SkipListTest, UpperBound) {
    for (int i = 0; i < 100; i += 2) {
        sl_int->insert(i);
    }
    
    auto ub = sl_int->upper_bound(35);
    EXPECT_NE(ub, sl_int->end());
    EXPECT_EQ(*ub, 36);
    
    ub = sl_int->upper_bound(98);
    EXPECT_EQ(ub, sl_int->end());
}

// String Type Tests
TEST_F(SkipListTest, StringInsertFind) {
    sl_string->insert("apple");
    sl_string->insert("banana");
    sl_string->insert("cherry");
    
    EXPECT_EQ(sl_string->size(), 3);
    EXPECT_NE(sl_string->find("banana"), sl_string->end());
    EXPECT_EQ(sl_string->find("date"), sl_string->end());
    EXPECT_TRUE(sl_string->validate());
}

// Copy/Move Tests
TEST_F(SkipListTest, CopyConstructor) {
    for (int i = 0; i < 10; ++i) {
        sl_int->insert(i);
    }
    
    SkipList<int> copy(*sl_int);
    EXPECT_EQ(copy.size(), sl_int->size());
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), sl_int->begin()));
    EXPECT_TRUE(copy.validate());
}

TEST_F(SkipListTest, MoveConstructor) {
    for (int i = 0; i < 10; ++i) {
        sl_int->insert(i);
    }
    
    SkipList<int> moved(std::move(*sl_int));
    EXPECT_EQ(moved.size(), 10);
    EXPECT_EQ(sl_int->size(), 0);
    EXPECT_TRUE(moved.validate());
}

// Stress Test
TEST_F(SkipListTest, LargeDataset) {
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        sl_int->insert(i);
    }
    
    EXPECT_EQ(sl_int->size(), N);
    EXPECT_TRUE(sl_int->validate());
    
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(sl_int->contains(i));
    }
    
    for (int i = 0; i < N; i += 2) {
        EXPECT_TRUE(sl_int->erase(i));
    }
    
    EXPECT_EQ(sl_int->size(), N/2);
    EXPECT_TRUE(sl_int->validate());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
