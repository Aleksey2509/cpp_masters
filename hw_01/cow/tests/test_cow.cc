#include <gtest/gtest.h>
#include <iterator>
#include <vector>
#include <ranges>
#include "cow.hh"

TEST(CowString, Basic)
{
    cow::cow_string first {"Cow is alive!"};
    ASSERT_EQ(first.size(), sizeof("Cow is alive!") - 1);
    ASSERT_GE(first.capacity(), first.size());
}

TEST(CowString, BasicCopy)
{
    cow::cow_string first {"Cow is alive!"};
    cow::cow_string second = first;

    second[2] = 'a';
    ASSERT_EQ(first[2], 'w');
    ASSERT_EQ(second[2], 'a');
}

TEST(CowString, IteratorTest)
{
    cow::cow_string str {"Hello"};
    std::string copied_via_iterators {str.begin(), str.end()};
    EXPECT_EQ(copied_via_iterators, "Hello");
}

TEST(CowString, ReverseIteratorTest)
{
    cow::cow_string str {"Hello"};
    std::string copied_via_iterators {str.rbegin(), str.rend()};
    EXPECT_EQ(copied_via_iterators, "olleH");
}

TEST(CowString, ResizeTest)
{
    cow::cow_string str {"First"};
    std::string_view expected {"Firstbbbbb"};
    str.resize(10, 'b');
    auto&& [sv_it, cow_it] = std::mismatch(expected.begin(), expected.end(), str.begin());
    EXPECT_EQ((sv_it == expected.end()) && (cow_it == str.end()), true)
        << "Expected {" << expected << "} got {" << str << "}\n";
}

TEST(CowString, SubstrTest)
{
    cow::cow_string str {"What a beautiful day!"};
    auto index_beautiful = str.find_substr("beautiful");
    auto index_a = str.find_substr(" a ");
    auto index_day = str.find_substr("day");
    EXPECT_EQ(index_beautiful, 7);
    EXPECT_EQ(index_a, 4);
    EXPECT_EQ(index_day, 17);
}

TEST(CowString, TokenizingTest)
{
    std::vector<cow::cow_string<char>> test_subjects {
        {"Cow string   wow"}, {"Hello new  or maybe|old exciting| world with cow| string   wow"}};
    std::vector<cow::cow_string<char>> delims {{" "}, {" |"}};
    std::vector<std::vector<std::string>> expected {
        {"Cow", "string", "wow"},
        {"Hello", "new", "or", "maybe", "old", "exciting", "world", "with", "cow", "string", "wow"}};
    
    for (int i = 0, test_amount = test_subjects.size(); i < test_amount; ++i)
    {
        auto tokenized = test_subjects[i].tokenize(std::string_view(delims[i]));
        std::vector<std::string> result;
        std::copy(tokenized.begin(), tokenized.end(), std::back_inserter(result));
        EXPECT_EQ(result, expected[i]) << "Failed for COW string {" << test_subjects[i] << "} with delimeters vec {"
                                        << delims[i] << "}\n";
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
