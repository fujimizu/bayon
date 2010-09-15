//
// Tests for Document class
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
#include "document.h"

/* Document::id */
TEST(DocumentTest, IdTest) {
  bayon::DocumentId id = 100;
  bayon::Document doc(id);
  EXPECT_EQ(doc.id(), id);
}

/* Document::add_feature */
TEST(DocumentTest, AddFeatureTest) {
  bayon::Document doc(1);
  std::vector<std::pair<bayon::VecKey, bayon::VecValue> > items;
  size_t max = 10;
  for (size_t i = 0; i < max; i++) {
    bayon::VecKey key = i;
    bayon::VecValue val = i * 10;
    items.push_back(std::pair<bayon::VecKey, bayon::VecValue>(key, val));
    doc.add_feature(key, val);
  }

  for (size_t i = 0; i < max; i++) {
    EXPECT_EQ(doc.feature()->get(items[i].first), items[i].second);
  }
}

/* Document::set_features */
TEST(DocumentTest, SetFeatureTest) {
  bayon::Document doc(1);
  bayon::Vector *vec = new bayon::Vector();
  size_t max = 10;
  for (size_t i = 0; i < max; i++) vec->set(i, i * 10);
  delete doc.feature();
  doc.set_features(vec);

  EXPECT_EQ(doc.feature()->size(), vec->size());
  const bayon::VecHashMap *hmap = vec->hash_map();
  for (bayon::VecHashMap::const_iterator it = hmap->begin(); it != hmap->end(); ++it) {
    EXPECT_EQ(doc.feature()->get(it->first), it->second);
  }
}

/* Document::clear */
TEST(DocumentTest, ClearTest) {
  bayon::Document doc(1);
  size_t max = 10;
  for (size_t i = 0; i < max; i++) doc.add_feature(i, i * 10);

  EXPECT_NE(0, doc.feature()->size());
  doc.clear();
  EXPECT_EQ(doc.feature()->size(), static_cast<size_t>(0));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
