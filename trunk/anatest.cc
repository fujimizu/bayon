//
// Tests for Analyzer class
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

#include <ctime>
#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "analyzer.h"

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
      double point = static_cast<double>(rand()) / RAND_MAX * MAX_POINT;
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

}  /* namespace */

/* Analyzer::idf */
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
  analyzer.do_clustering(bayon::Analyzer::RB);

  std::map<bayon::DocumentId, size_t> choosed;
  int count = 0;
  bayon::Cluster cluster;
  while (analyzer.get_next_result(cluster)) {
    ++count;
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
  analyzer.do_clustering(bayon::Analyzer::KMEANS);

  std::map<bayon::DocumentId, size_t> choosed;
  int count = 0;
  bayon::Cluster cluster;
  while (analyzer.get_next_result(cluster)) {
    ++count;
    for (size_t i = 0; i < cluster.size(); i++) {
      EXPECT_TRUE(choosed.find(cluster.documents()[i]->id()) == choosed.end());
      choosed[cluster.documents()[i]->id()] = count;
    }
  }
  EXPECT_EQ(count, nclusters);
  delete_documents(documents);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
