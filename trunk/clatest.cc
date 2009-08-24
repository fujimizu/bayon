//
// Tests for Classifier
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

#include <ctime>
#include <map>
#include <gtest/gtest.h>
#include "classifier.h"

namespace {

const size_t NUM_VECTOR_ITEM = 5;

void add_random_vectors(size_t max_vec, size_t max_key,
                        bayon::Classifier &classifier) {
  for (size_t i = 0; i < max_vec; i++) {
    bayon::Vector vec;
    for (size_t j = 0; j < max_key; j++) {
      vec.set(j, j);
    }
    classifier.add_vector(i, vec);
  }
}

TEST(ClassifierTest, AddVectorTest) {
  bayon::Classifier classifier;
  size_t max = 10;
  add_random_vectors(max, NUM_VECTOR_ITEM, classifier);
  EXPECT_EQ(classifier.count_vectors(), max);
}

TEST(ClassifierTest, SimilarVectorsTest) {
  bayon::Classifier classifier;
  size_t max = 10;
  add_random_vectors(max, NUM_VECTOR_ITEM, classifier);

  std::vector<std::pair<bayon::VectorId, double> > items;
  bayon::Vector vec;
  for (size_t i = 0; i < NUM_VECTOR_ITEM; i++) {
    vec.set(i, rand() % 10 + 1);
  }
  vec.normalize();
  classifier.similar_vectors(NUM_VECTOR_ITEM, vec, items);

  EXPECT_TRUE(0 < items.size() && items.size() <= max);
  std::map<bayon::VectorId, bool> check;
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_TRUE(-1.0 <= items[i].second && items[i].second <= 1.0);
    EXPECT_TRUE(check.find(items[i].first) == check.end());
    check[items[i].first] = true;
  }
}

} /* namespace */

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
