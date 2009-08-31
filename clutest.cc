//
// Tests for Clustering API
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
#include <iostream>
#include <iterator>
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

  EXPECT_TRUE(doc.feature()->size() != 0);
  doc.clear();
  EXPECT_EQ(doc.feature()->size(), static_cast<size_t>(0));
}

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

  bayon::Vector compvec = *cluster.composite_vector();
  EXPECT_EQ(compvec.size(), vec.size());

  std::vector<bayon::VecItem> items;
  compvec.sorted_items_abs(items);
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

  bayon::Vector centvec = *cluster.centroid_vector();
  EXPECT_EQ(centvec.size(), vec.size());

  std::vector<bayon::VecItem> items;
  centvec.sorted_items_abs(items);
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

/* Analzyer::idf */
TEST(AnalyzerTest, IdfTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Analyzer analyzer;

  std::vector<bayon::Vector> vectors_org;
  bayon::HashMap<bayon::VecKey, size_t>::type df;
  bayon::init_hash_map(bayon::VECTOR_EMPTY_KEY, df);
  for (size_t i = 0; i < documents.size(); i++) {
    bayon::Vector vec;
    bayon::VecHashMap *hmap = documents[i]->feature()->hash_map();
    for (bayon::VecHashMap::iterator it = hmap->begin();
         it != hmap->end(); ++it) {
      df[it->first]++;
    }
    documents[i]->feature()->copy(vec);
    vectors_org.push_back(vec);
    analyzer.add_document(*documents[i]);
  }

  analyzer.idf();

  std::vector<bayon::Document *> documents_idf = analyzer.documents();
  for (size_t i = 0; i < documents_idf.size(); i++) {
    bayon::VecHashMap *hmap = documents_idf[i]->feature()->hash_map();
    for (bayon::VecHashMap::iterator it = hmap->begin();
         it != hmap->end(); ++it) {
      double val = vectors_org[i].get(it->first) *
                   log((double)vectors_org.size() / df[it->first]);
      EXPECT_EQ(it->second, val);
    }
  }

  delete_documents(documents);
}

/* Analyzer::do_clustering(RB) */
TEST(AnalyzerTest, DoClusteringRBTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Analyzer analyzer;

  for (size_t i = 0; i < documents.size(); i++) {
    analyzer.add_document(*documents[i]);
  }
  int nclusters = 2;
  analyzer.set_cluster_size_limit(nclusters);
  analyzer.do_clustering("rb");

  std::map<bayon::DocumentId, size_t> choosed;
  int count = 0;
  bayon::Cluster cluster;
  while (analyzer.get_next_result(cluster)) {
    ++count;
    //std::cout << cluster << std::endl;
    for (size_t i = 0; i < cluster.size(); i++) {
      EXPECT_TRUE(choosed.find(cluster.documents()[i]->id()) == choosed.end());
      choosed[cluster.documents()[i]->id()] = count;
    }
  }
  EXPECT_EQ(count, nclusters);
  delete_documents(documents);
}

/* Analyzer::do_clustering(k-means) */
TEST(AnalyzerTest, DoClusteringKmeansTest) {
  std::vector<bayon::Document *> documents;
  init_documents(documents);
  bayon::Analyzer analyzer;

  for (size_t i = 0; i < documents.size(); i++) {
    analyzer.add_document(*documents[i]);
  }
  int nclusters = 2;
  analyzer.set_cluster_size_limit(nclusters);
  analyzer.do_clustering("kmeans");

  std::map<bayon::DocumentId, size_t> choosed;
  int count = 0;
  bayon::Cluster cluster;
  while (analyzer.get_next_result(cluster)) {
    ++count;
    //std::cout << cluster << std::endl;
    for (size_t i = 0; i < cluster.size(); i++) {
      EXPECT_TRUE(choosed.find(cluster.documents()[i]->id()) == choosed.end());
      choosed[cluster.documents()[i]->id()] = count;
    }
  }
  EXPECT_EQ(count, nclusters);
  delete_documents(documents);
}

} /* namespace */

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
