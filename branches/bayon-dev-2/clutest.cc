//
// Tests for Clustering API
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


#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "cluster.h"

using namespace bayon;

namespace {

std::vector<Document *> documents;

const size_t NUM_DOCUMENT   = 10;
const size_t NUM_FEATURE    = 5;
const size_t MAX_FEATURE_ID = 10;
const double MAX_POINT      = 10.0;

void init_documents() {
  for (size_t i = 0; i < NUM_DOCUMENT; i++) {
    Document *doc = new Document(i);
    for (size_t j = 0; j < NUM_FEATURE; j++) {
      size_t id = rand() % MAX_FEATURE_ID;
      double point = rand() / (double)RAND_MAX * MAX_POINT;
      doc->add_feature(id, point);
    }
    documents.push_back(doc);
  }
}

void delete_documents() {
  for (size_t i = 0; i < documents.size(); i++) {
    delete documents[i];
  }
  documents.clear();
}

void set_cluster(Cluster &cluster) {
  for (size_t i = 0; i < documents.size(); i++) {
    cluster.add_document(documents[i]);
  }
}

/* Document::id */
TEST(DocumentTest, IdTest) {
  DocumentId id = 100;
  Document doc(id);
  EXPECT_EQ(doc.id(), id);
}

/* Document::add_feature */
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

/* Document::set_feature */
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

/* Document::clear */
TEST(DocumentTest, ClearTest) {
  Document doc(1);
  size_t max = 10;
  for (size_t i = 0; i < max; i++) doc.add_feature(i, i * 10);

  EXPECT_TRUE(doc.feature()->size() != 0);
  doc.clear();
  EXPECT_EQ(doc.feature()->size(), 0);
}

/* Cluster::size */
TEST(ClusterTest, SizeTest) {
  init_documents();
  Document d1(1), d2(2), d3(3);
  Cluster cluster;
  cluster.add_document(&d1);
  cluster.add_document(&d2);
  cluster.add_document(&d3);
  EXPECT_EQ(cluster.size(), 3);
  delete_documents();
}

/* Cluster::composite_vector */
TEST(ClusterTest, CompositeTest) {
  init_documents();
  Cluster cluster;
  set_cluster(cluster);

  Vector vec;
  for (size_t i = 0; i < documents.size(); i++) {
    vec.add_vector(*documents[i]->feature());
  }

  Vector compvec = *cluster.composite_vector();
  EXPECT_EQ(compvec.size(), vec.size());

  std::vector<VecItem> items;
  compvec.sorted_items(items);
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].second, vec.get(items[i].first));
  }
  delete_documents();
}

/* Cluster::centroid_vector */
TEST(ClusterTest, CentroidTest) {
  init_documents();
  Cluster cluster;
  set_cluster(cluster);

  Vector vec;
  for (size_t i = 0; i < documents.size(); i++) {
    vec.add_vector(*documents[i]->feature());
  }
  vec.normalize();

  Vector centvec = *cluster.centroid_vector();
  EXPECT_EQ(centvec.size(), vec.size());

  std::vector<VecItem> items;
  centvec.sorted_items(items);
  for (size_t i = 0; i < items.size(); i++) {
    EXPECT_EQ(items[i].second, vec.get(items[i].first));
  }
  delete_documents();
}

/* Cluster::choose_randomly */
TEST(ClusterTest, ChooseRandomlyTest) {
  init_documents();
  Cluster cluster;
  set_cluster(cluster);

  for (size_t i = 0; i < 100; i++) {
    std::vector<Document *> docs;
    size_t ndocs = 3;
    cluster.choose_randomly(ndocs, docs);
    EXPECT_EQ(docs.size(), ndocs);

    std::map<DocumentId, bool> choosed;
    for (size_t j = 0; j < docs.size(); j++) {
      EXPECT_TRUE(choosed.find(docs[j]->id()) == choosed.end());
      choosed[docs[j]->id()] = true;
    }
  }
  delete_documents();
}

/* Cluster::choose_smartly */
TEST(ClusterTest, ChooseSmartlyTest) {
  init_documents();
  Cluster cluster;
  set_cluster(cluster);

  for (size_t i = 0; i < 100; i++) {
    std::vector<Document *> docs;
    size_t ndocs = 3;
    cluster.choose_smartly(ndocs, docs);
    EXPECT_EQ(docs.size(), ndocs);

    std::map<DocumentId, bool> choosed;
    for (size_t j = 0; j < docs.size(); j++) {
      EXPECT_TRUE(choosed.find(docs[j]->id()) == choosed.end());
      choosed[docs[j]->id()] = true;
    }
  }
  delete_documents();
}

/* Cluster::section */
TEST(ClusterTest, SectionTest) {
  init_documents();
  Cluster cluster;
  set_cluster(cluster);

  size_t nclusters = 2;
  cluster.section(nclusters);
  std::vector<Cluster *> clusters = cluster.sectioned_clusters();
  EXPECT_EQ(clusters.size(), nclusters);

  std::map<DocumentId, size_t> choosed;
  for (size_t i = 0; i < clusters.size(); i++) {
    for (size_t j = 0; j < clusters[i]->size(); j++) {
      EXPECT_TRUE(choosed.find(clusters[i]->documents()[j]->id()) == choosed.end());
      choosed[clusters[i]->documents()[j]->id()] = i;
    }
  }

  for (size_t i = 0; i < cluster.sectioned_clusters().size(); i++) {
    delete cluster.sectioned_clusters()[i];
  }
  delete_documents();
}

/* Analyzer::do_clustering(RB) */
TEST(AnalyzerTest, DoClusteringRBTest) {
  init_documents();
  Analyzer analyzer;

  for (size_t i = 0; i < documents.size(); i++) {
    analyzer.add_document(*documents[i]);
  }
  int nclusters = 2;
  analyzer.set_cluster_size_limit(nclusters);
  analyzer.do_clustering("rb");

  std::map<DocumentId, size_t> choosed;
  int count = 0;
  Cluster cluster;
  while (analyzer.get_next_result(cluster)) {
    ++count;
    //std::cout << cluster << std::endl;
    for (size_t i = 0; i < cluster.size(); i++) {
      EXPECT_TRUE(choosed.find(cluster.documents()[i]->id()) == choosed.end());
      choosed[cluster.documents()[i]->id()] = count;
    }
  }
  EXPECT_EQ(count, nclusters);
  delete_documents();
}

/* Analyzer::do_clustering(k-means) */
TEST(AnalyzerTest, DoClusteringKmeansTest) {
  init_documents();
  Analyzer analyzer;

  for (size_t i = 0; i < documents.size(); i++) {
    analyzer.add_document(*documents[i]);
  }
  int nclusters = 2;
  analyzer.set_cluster_size_limit(nclusters);
  analyzer.do_clustering("kmeans");

  std::map<DocumentId, size_t> choosed;
  int count = 0;
  Cluster cluster;
  while (analyzer.get_next_result(cluster)) {
    ++count;
    //std::cout << cluster << std::endl;
    for (size_t i = 0; i < cluster.size(); i++) {
      EXPECT_TRUE(choosed.find(cluster.documents()[i]->id()) == choosed.end());
      choosed[cluster.documents()[i]->id()] = count;
    }
  }
  EXPECT_EQ(count, nclusters);
  delete_documents();
}

} /* namespace */

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
