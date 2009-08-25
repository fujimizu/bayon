//
// Tests for vector operation
//
// Copyright(C) 2009  Mizuki Fujisawa <fujisawa@bayon.cc>
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

#include <map>
#include <gtest/gtest.h>
#include "byvector.h"

namespace {

typedef std::map<bayon::VecKey, bayon::VecValue> TestData;

/* function prototypes */
static void init_vector(bayon::Vector &vec, const TestData &m);
static void set_input_values();

/* global variables */
TestData input1, input2, input3, input4, normalized1, normalized2;
std::vector<bayon::VecKey> sorted_keys, sorted_keys_abs;

/* initialize vector */
static void init_vector(bayon::Vector &vec, const TestData &m) {
  vec.clear();
  for (TestData::const_iterator it = m.begin(); it != m.end(); ++it) {
    vec.set(it->first, it->second);
  }
}

/* set test data */
static void set_input_values() {
  input1[1] = 1.0;
  input1[2] = 2.0;
  input1[3] = 3.0;

  sorted_keys.push_back(3);
  sorted_keys.push_back(2);
  sorted_keys.push_back(1);

  normalized1[1] = 1.0 / sqrt(1 * 1 + 2 * 2 + 3 * 3);
  normalized1[2] = 2.0 / sqrt(1 * 1 + 2 * 2 + 3 * 3);
  normalized1[3] = 3.0 / sqrt(1 * 1 + 2 * 2 + 3 * 3);

  input2[1] = 3.0;
  input2[2] = 6.0;
  input2[3] = 9.0;

  input3[1] = 3.0;
  input3[2] = 2.0;
  input3[3] = 1.0;

  input4[1] = 1.0;
  input4[2] = -2.0;
  input4[3] = 3.0;

  sorted_keys_abs.push_back(3);
  sorted_keys_abs.push_back(2);
  sorted_keys_abs.push_back(1);
}

} // namespace

/* copy */
TEST(VectorTest, CopyTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  vec1.copy(vec2);

  std::vector<bayon::VecItem> items;
  vec1.sorted_items(items);
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].second, vec2.get(items[i].first));
  }
}

/* get, set */
TEST(VectorTest, GetSetTest) {
  bayon::Vector vec;
  for (bayon::VecKey key = 0; key < 10; key++) {
    vec.set(key, key * 10);
  }
  for (bayon::VecKey key = 0; key < 10; key++) {
    EXPECT_EQ(vec.get(key), key * 10);
  }
}

/* size */
TEST(VectorTest, SizeTest) {
  bayon::Vector vec;
  init_vector(vec, input1);
  EXPECT_EQ(vec.size(), input1.size());
}

/* clear */
TEST(VectorTest, ClearTest) {
  bayon::Vector vec;
  init_vector(vec, input1);
  vec.clear();
  EXPECT_EQ(vec.size(), static_cast<size_t>(0));
}

/* hash_map */
TEST(VectorTest, HashMapTest) {
  bayon::Vector vec;
  std::vector<bayon::VecKey> keys;
  for (TestData::iterator it = input1.begin(); it != input1.end(); ++it) {
    keys.push_back(it->first);
  }
  reverse(keys.begin(), keys.end());
  TestData reversed;
  for (size_t i = 0; i < keys.size(); i++) {
    reversed[keys[i]] = input1[keys[i]];
  }
  init_vector(vec, reversed);

  const bayon::VecHashMap *hash = vec.hash_map();
  for (TestData::const_iterator it = input1.begin(); it != input1.end(); ++it) {
    bayon::VecHashMap::const_iterator ith = hash->find(it->first);
    EXPECT_EQ(it->second, ith->second);
  }
}

/* sorted_items */
TEST(VectorTest, SortedItemsTest) {
  bayon::Vector vec;
  init_vector(vec, input1);
  std::vector<bayon::VecItem> items;
  vec.sorted_items(items);
  EXPECT_EQ(items.size(), sorted_keys.size());
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].first, sorted_keys[i]);
  }
}

/* sorted_items_abs */
TEST(VectorTest, SortedItemsAbsTest) {
  bayon::Vector vec;
  init_vector(vec, input4);
  std::vector<bayon::VecItem> items;
  vec.sorted_items_abs(items);
  EXPECT_EQ(items.size(), sorted_keys_abs.size());
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].first, sorted_keys_abs[i]);
  }
}

/* normalize */
TEST(VectorTest, NormalizeTest) {
  bayon::Vector vec;
  init_vector(vec, input1);
  vec.normalize();
  for (TestData::iterator it = normalized1.begin();
       it != normalized1.end(); ++it) {
    EXPECT_EQ(it->second, vec.get(it->first));
  }
}

/* resize */
TEST(VectorTest, ResizeTest) {
  bayon::Vector vec;
  for (bayon::VecKey i = 1; i <= 100; i++) {
    vec.set(i, i);
  }
  size_t size = 30;
  vec.resize(size);
  EXPECT_EQ(vec.size(), size);
}

/* norm */
TEST(VectorTest, NormTest) {
  bayon::Vector vec;
  init_vector(vec, input1);
  double norm = vec.norm();
  EXPECT_EQ(sqrt(1 * 1 + 2 * 2 + 3 * 3), norm);
}

/* multiply_constant */
TEST(VectorTest, MultiplyConstantTest) {
  bayon::Vector vec;
  init_vector(vec, input1);
  double x = 3;
  vec.multiply_constant(x);
  for (TestData::iterator it = input1.begin();
       it != input1.end(); ++it) {
    EXPECT_EQ(it->second * x, vec.get(it->first));
  }
}

/* add_vector */
TEST(VectorTest, AddVectorTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  init_vector(vec2, input2);
  vec1.add_vector(vec2);
  for (TestData::iterator it = input1.begin();
       it != input1.end(); ++it) {
    EXPECT_EQ(it->second + input2[it->first], vec1.get(it->first));
  }
}

/* delete_vector */
TEST(VectorTest, DeleteVectorTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  init_vector(vec2, input2);
  vec1.delete_vector(vec2);
  for (TestData::iterator it = input1.begin();
       it != input1.end(); ++it) {
    EXPECT_EQ(it->second - input2[it->first], vec1.get(it->first));
  }
}

/* euclid_distance */
TEST(VectorTest, EuclidDistanceTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  init_vector(vec2, input2);
  double dist = bayon::Vector::euclid_distance(vec1, vec2);
  EXPECT_EQ(dist, sqrt((1-3)*(1-3) + (2-6)*(2-6) + (3-9)*(3-9)));
}

/* inner_product */
TEST(VectorTest, InnerProductTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  init_vector(vec2, input2);
  double dist = bayon::Vector::inner_product(vec1, vec2);
  EXPECT_EQ(dist, (1*3 + 2*6 + 3*9));
}

/* cosine */
TEST(VectorTest, CosineTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  init_vector(vec2, input2);
  double cos = bayon::Vector::cosine(vec1, vec2);
  EXPECT_EQ(
    cos,
    (1*3 + 2*6 + 3*9) / (sqrt(1*1 + 2*2 + 3*3) * sqrt(3*3 + 6*6 + 9*9)));
}

/* jaccard */
TEST(VectorTest, JaccardTest) {
  bayon::Vector vec1, vec2;
  init_vector(vec1, input1);
  init_vector(vec2, input3);
  double jaccard = bayon::Vector::jaccard(vec1, vec2);

  double prod = bayon::Vector::inner_product(vec1, vec2);
  double norm1 = vec1.norm();
  double norm2 = vec2.norm();
  double denom = norm1 + norm2 - prod;
  if (!denom) {
    EXPECT_EQ(jaccard, 0);
  } else {
    EXPECT_EQ((int)(jaccard * 10), (int)(prod/denom *  10));
  }
}

/* main function */
int main(int argc, char **argv) {
  set_input_values();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
