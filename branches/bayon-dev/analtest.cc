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

#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "analyzer.h"

using namespace bayon;

namespace {

const size_t NUM_FEATURE    = 5;
const size_t MAX_FEATURE_ID = 10;
const double MAX_POINT      = 10.0;

void set_random_documents(size_t ndocs, std::vector<Document *> &documents) {
  for (size_t i = 0; i < ndocs; i++) {
    Document *doc = new Document(i);
    for (size_t j = 0; j < NUM_FEATURE; j++) {
      size_t id = rand() % MAX_FEATURE_ID;
      double point = rand() / (double)RAND_MAX * MAX_POINT;
      doc->add_feature(id, point);
    }
    documents.push_back(doc);
  }
}

void delete_documents(std::vector<Document *> &documents) {
  for (size_t i = 0; i < documents.size(); i++) {
    delete documents[i];
  }
  documents.clear();
}

TEST(AnalyzerTest, DoClusteringRBTest) {
  std::vector<Document *> documents;
  set_random_documents(10, documents);
  Analyzer analyzer;

  for (size_t i = 0; i < documents.size(); i++) {
    analyzer.add_document(*documents[i]);
  }
  int nclusters = 2;
  analyzer.set_cluster_size_limit(nclusters);
  analyzer.do_clustering(REPEATED_BISECTION);

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
  delete_documents(documents);
}

TEST(AnalyzerTest, DoClusteringKmeansTest) {
  std::vector<Document *> documents;
  set_random_documents(10, documents);
  Analyzer analyzer;

  for (size_t i = 0; i < documents.size(); i++) {
    analyzer.add_document(*documents[i]);
  }
  int nclusters = 2;
  analyzer.set_cluster_size_limit(nclusters);
  analyzer.do_clustering(KMEANS);

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
  delete_documents(documents);
}

} /* namespace */

/* main function */
int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
