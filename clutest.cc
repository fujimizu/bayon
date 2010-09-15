//
// Tests for Cluster class
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
#include <vector>
#include <gtest/gtest.h>
#include "cluster.h"

namespace {

const size_t NUM_DOCUMENT   = 10;
const size_t NUM_FEATURE    = 5;
const size_t MAX_FEATURE_ID = 10;
const double MAX_POINT      = 10.0;

void init_documents(std::vector<bayon::Document *> &documents) {
  for (size_t i = 0; i < NUM_DOCUMENT; i++) {
    bayon::Document *doc = new bayon::Document(i);
    for (size_t j = 0; j < NUM_FEATURE; j++) {
      size_t id = rand() % MAX_FEATURE_ID;
      double point = rand() / (double)RAND_MAX * MAX_POINT;
      doc->add_feature(id, point);
    }
    documents.push_back(doc);
  }
}

void delete_documents(std::vector<bayon::Document *> &documents) {
  for (size_t i = 0; i < documents.size(); i++) {
    delete documents[i];
  }
  documents.clear();
}

void set_cluster(bayon::Cluster &cluster,
                 std::vector<bayon::Document *> &documents) {
  for (size_t i = 0; i < documents.size(); i++) {
    cluster.add_document(documents[i]);
  }
}

} /* namespace */

/* Cluster::size */
TEST(ClusterTest, SizeTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Document d1(1), d2(2), d3(3);
  bayon::Cluster cluster;
  cluster.add_document(&d1);
  cluster.add_document(&d2);
  cluster.add_document(&d3);
  EXPECT_EQ(cluster.size(), static_cast<size_t>(3));
  delete_documents(documents);
}

/* Cluster::composite_vector */
TEST(ClusterTest, CompositeTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Cluster cluster;
  set_cluster(cluster, documents);

  bayon::Vector vec;
  for (size_t i = 0; i < documents.size(); i++) {
    vec.add_vector(*documents[i]->feature());
  }

  bayon::Vector *compvec = cluster.composite_vector();
  EXPECT_EQ(compvec->size(), vec.size());

  std::vector<bayon::VecItem> items;
  compvec->sorted_items_abs(items);
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].second, vec.get(items[i].first));
  }
  delete_documents(documents);
}

/* Cluster::centroid_vector */
TEST(ClusterTest, CentroidTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Cluster cluster;
  set_cluster(cluster, documents);

  bayon::Vector vec;
  for (size_t i = 0; i < documents.size(); i++) {
    vec.add_vector(*documents[i]->feature());
  }
  vec.normalize();

  bayon::Vector *centvec = cluster.centroid_vector();
  EXPECT_EQ(centvec->size(), vec.size());

  std::vector<bayon::VecItem> items;
  centvec->sorted_items_abs(items);
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].second, vec.get(items[i].first));
  }
  delete_documents(documents);
}

/* Cluster::choose_randomly */
TEST(ClusterTest, ChooseRandomlyTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Cluster cluster;
  set_cluster(cluster, documents);

  for (size_t i = 0; i < 100; i++) {
    std::vector<bayon::Document *> docs;
    size_t ndocs = 3;
    cluster.choose_randomly(ndocs, docs);
    EXPECT_EQ(docs.size(), ndocs);

    std::map<bayon::DocumentId, bool> choosed;
    for (size_t j = 0; j < docs.size(); j++) {
      EXPECT_TRUE(choosed.find(docs[j]->id()) == choosed.end());
      choosed[docs[j]->id()] = true;
    }
  }
  delete_documents(documents);
}

/* Cluster::choose_smartly */
TEST(ClusterTest, ChooseSmartlyTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Cluster cluster;
  set_cluster(cluster, documents);

  for (size_t i = 0; i < 100; i++) {
    std::vector<bayon::Document *> docs;
    size_t ndocs = 3;
    cluster.choose_smartly(ndocs, docs);
    EXPECT_EQ(docs.size(), ndocs);

    std::map<bayon::DocumentId, bool> choosed;
    for (size_t j = 0; j < docs.size(); j++) {
      EXPECT_TRUE(choosed.find(docs[j]->id()) == choosed.end());
      choosed[docs[j]->id()] = true;
    }
  }
  delete_documents(documents);
}

/* Cluster::section */
TEST(ClusterTest, SectionTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Cluster cluster;
  set_cluster(cluster, documents);

  size_t nclusters = 2;
  cluster.section(nclusters);
  std::vector<bayon::Cluster *> clusters = cluster.sectioned_clusters();
  EXPECT_EQ(clusters.size(), nclusters);

  std::map<bayon::DocumentId, size_t> choosed;
  for (size_t i = 0; i < clusters.size(); i++) {
    for (size_t j = 0; j < clusters[i]->size(); j++) {
      EXPECT_TRUE(choosed.find(clusters[i]->documents()[j]->id()) == choosed.end());
      choosed[clusters[i]->documents()[j]->id()] = i;
    }
  }

  for (size_t i = 0; i < cluster.sectioned_clusters().size(); i++) {
    delete cluster.sectioned_clusters()[i];
  }
  delete_documents(documents);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
