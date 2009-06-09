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

void init_documents() {
  Document *d1 = new Document(1);
  d1->add_feature(1, 1.0);
  d1->add_feature(2, 1.0);
  d1->add_feature(3, 1.0);
  d1->feature()->normalize();
  documents.push_back(d1);

  Document *d2 = new Document(2);
  d2->add_feature(1, 1.0);
  d2->add_feature(3, 1.0);
  d2->feature()->normalize();
  documents.push_back(d2);
  
  Document *d3 = new Document(3);
  d3->add_feature(1, 1.0);
  d3->add_feature(2, 1.0);
  d3->feature()->normalize();
  documents.push_back(d3);

  Document *d4 = new Document(4);
  d4->add_feature(4, 1.0);
  d4->add_feature(5, 1.0);
  d4->add_feature(6, 1.0);
  d4->feature()->normalize();
  documents.push_back(d4);

  Document *d5 = new Document(5);
  d5->add_feature(4, 1.0);
  d5->add_feature(6, 1.0);
  d5->feature()->normalize();
  documents.push_back(d5);

  Document *d6 = new Document(6);
  d6->add_feature(4, 1.0);
  d6->add_feature(5, 1.0);
  d6->feature()->normalize();
  documents.push_back(d6);
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

TEST(DocumentTest, SizeTest) {
  init_documents();
  Document doc(1);
  size_t max = 10;
  for (size_t i = 0; i < max; i++) {
    doc.add_feature(i, i * 2.0);
  }
  EXPECT_EQ(doc.feature()->size(), 10);

  doc.clear();
  EXPECT_EQ(doc.feature()->size(), 0);
  delete_documents();
}

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
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
