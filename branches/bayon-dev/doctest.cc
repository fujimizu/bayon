//
// Tests for Document class
//
// Copyright(C) 2009  Mizuki Fujisawa <mfujisa@gmail.com>
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

#include <utility>
#include <gtest/gtest.h>
#include "document.h"

using namespace bayon;

namespace {

/* id */
TEST(DocumentTest, IdTest) {
  DocumentId id = 100;
  Document doc(id);
  EXPECT_EQ(doc.id(), id);
}

/* add_feature */
TEST(DocumentTest, AddFeatureTest) {
  Document doc(1);
  std::vector<std::pair<VecKey, VecValue> > items;
  size_t max = 10;
  for (size_t i = 0; i < max; i++) {
    VecKey key = i;
    VecValue val = i * 10;
    items.push_back(std::pair<VecKey, VecValue>(key, val));
    doc.add_feature(key, val);
  }

  for (size_t i = 0; i < max; i++) {
    EXPECT_EQ(doc.feature()->get(items[i].first), items[i].second);
  }
}

/* set_feature */
TEST(DocumentTest, SetFeatureTest) {
  Document doc(1);
  Vector *vec = new Vector();
  size_t max = 10;
  for (size_t i = 0; i < max; i++) vec->set(i, i * 10);
  delete doc.feature();
  doc.set_feature(vec);

  EXPECT_EQ(doc.feature()->size(), vec->size());
  const VecHashMap *hmap = vec->hash_map();
  for (VecHashMap::const_iterator it = hmap->begin(); it != hmap->end(); ++it) {
    EXPECT_EQ(doc.feature()->get(it->first), it->second);
  }
}

/* clear */
TEST(DocumentTest, ClearTest) {
  Document doc(1);
  size_t max = 10;
  for (size_t i = 0; i < max; i++) doc.add_feature(i, i * 10);

  EXPECT_TRUE(doc.feature()->size() != 0);
  doc.clear();
  EXPECT_EQ(doc.feature()->size(), 0);
}

} /* namespace */

/* main function */
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
