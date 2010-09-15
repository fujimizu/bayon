//
// Tests for Utility functions
//
// Copyright(C) 2010  Mizuki Fujisawa <fujisawa@bayon.cc>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include <gtest/gtest.h>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <utility>
#include <vector>
#include "util.h"

namespace {

/* constants */
const size_t NUM_PAIRS     = 100;     ///< number of pairs
const size_t NUM_INTEGERS  = 10;      ///< number of integers

/* function prototypes */
static void random_pairs(size_t size,
                         std::vector<std::pair<int, double> > &pairs);

/* set random pairs */
static void random_pairs(size_t size,
                         std::vector<std::pair<int, double> > &pairs) {
  for (size_t i = 0; i < size; i++) {
    std::pair<int, double> p;
    p.first = i;
    p.second = rand();
    if (i % 2 == 0) p.second *= -1;
    pairs.push_back(p);
  }
}

} /* namespace */


/* greater_pair */
TEST(UtilTest, ComparePairItemsTest) {
  std::vector<std::pair<int, double> > pairs;
  random_pairs(NUM_PAIRS, pairs);
  std::sort(pairs.begin(), pairs.end(), bayon::greater_pair<int, double>);
  for (size_t i = 1; i < pairs.size(); i++) {
    EXPECT_TRUE(pairs[i-1].second >= pairs[i].second);
  }
}

/* greater_pair */
TEST(UtilTest, ComparePairItemsSameValueTest) {
  std::vector<std::pair<int, int> > pairs;
  pairs.push_back(std::pair<int, int>(1, 1));
  pairs.push_back(std::pair<int, int>(2, 1));
  std::sort(pairs.begin(), pairs.end(), bayon::greater_pair<int, int>);
  EXPECT_TRUE(pairs[0].first > pairs[1].first);
}

/* greater_pair_abs */
TEST(UtilTest, ComparePairItemsAbsTest) {
  std::vector<std::pair<int, double> > pairs;
  random_pairs(NUM_PAIRS, pairs);
  std::sort(pairs.begin(), pairs.end(), bayon::greater_pair_abs<int, double>);
  for (size_t i = 1; i < pairs.size(); i++) {
    EXPECT_TRUE(std::abs(pairs[i-1].second) >= std::abs(pairs[i].second));
  }
}

/* greater_pair_abs */
TEST(UtilTest, ComparePairItemsAbsSameValueTest) {
  std::vector<std::pair<int, int> > pairs;
  pairs.push_back(std::pair<int, int>(1, 1));
  pairs.push_back(std::pair<int, int>(2, -1));
  std::sort(pairs.begin(), pairs.end(), bayon::greater_pair_abs<int, int>);
  EXPECT_TRUE(pairs[0].first > pairs[1].first);
}

/* get_extention */
TEST(UtilTest, GetExtensionTest) {
  std::string filename;
  filename = "test.tsv";
  EXPECT_EQ("tsv", bayon::get_extension(filename));

  filename = "test.tsv.txt";
  EXPECT_EQ("txt", bayon::get_extension(filename));

  filename = "test.tsv.";
  EXPECT_EQ("", bayon::get_extension(filename));

  filename = "test";
  EXPECT_EQ("", bayon::get_extension(filename));
}

/* split_string */
TEST(UtilTest, SplitStringTest) {
  std::string input;
  std::vector<std::string> splited;

  // space delimiter
  input = "This is a pen";
  bayon::split_string(input, " ", splited);
  EXPECT_EQ(4, splited.size());
  EXPECT_EQ("This", splited[0]);
  EXPECT_EQ("is",   splited[1]);
  EXPECT_EQ("a",    splited[2]);
  EXPECT_EQ("pen",  splited[3]);
  splited.clear();

  // tab delimiter
  input = "This\tis\ta\tpen";
  bayon::split_string(input, "\t", splited);
  EXPECT_EQ(4, splited.size());
  EXPECT_EQ("This", splited[0]);
  EXPECT_EQ("is",   splited[1]);
  EXPECT_EQ("a",    splited[2]);
  EXPECT_EQ("pen",  splited[3]);
  splited.clear();

  // space delimiter
  input = "あい うえ おか";
  bayon::split_string(input, " ", splited);
  EXPECT_EQ(3, splited.size());
  EXPECT_EQ("あい", splited[0]);
  EXPECT_EQ("うえ",   splited[1]);
  EXPECT_EQ("おか",    splited[2]);
  splited.clear();

  // tab delimiter
  input = "あい\tうえ\tおか";
  bayon::split_string(input, "\t", splited);
  EXPECT_EQ(3, splited.size());
  EXPECT_EQ("あい", splited[0]);
  EXPECT_EQ("うえ",   splited[1]);
  EXPECT_EQ("おか",    splited[2]);
  splited.clear();
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
