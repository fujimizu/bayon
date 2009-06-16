//
// Analyzer
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

#ifndef BAYON_ANALYZER_H
#define BAYON_ANALYZER_H

#include <vector>
#include "byvector.h"
#include "cluster.h"
#include "document.h"

namespace bayon {

/**
 * Analyzer class
 *
 * Analyzer manages input documents, and do clustering
 */
class Analyzer {
 private:
  /**
   * max count of cluster refinement loop
   */
  static const unsigned int NUM_REFINE_LOOP = 30;

  /**
   * target documents
   */
  std::vector<Document *> documents_;

  /**
   * clustering result
   */
  std::vector<Cluster *> clusters_;

  /**
   * index of cluster
   */
  size_t cluster_index_;

  /**
   * number of clusters
   */
  size_t limit_nclusters_;

  /**
   * limit of evaluation
   */
  double limit_eval_;

  /**
   * Do repeated bisection clustering
   *
   * @return size_t number of clusters
   */
  size_t repeated_bisection();

  /**
   * Do k-means clustering
   *
   * @return size_t number of clusters
   */
  size_t kmeans();

  /**
   * Refine clustering result
   *
   * @param clusters clusters that will be refined
   * @param func criterion function
   * @return double value of refiend clusters
   */
  double refine_clusters(std::vector<Cluster *> &clusters);

  inline double refined_vector_value(const Vector &composite,
                                     const Vector &vec, int sign);

 public:
  /**
   * Constructor
   */
  Analyzer() : cluster_index_(0), limit_nclusters_(0), limit_eval_(-1.0) { }

  /**
   * Destructor
   */
  ~Analyzer() {
    for (size_t i = 0; i < documents_.size(); i++) {
      delete documents_[i];
    }
    for (size_t i = 0; i < clusters_.size(); i++) {
      for (size_t j = 0; j < clusters_[i]->sectioned_clusters().size(); j++) {
        delete clusters_[i]->sectioned_clusters()[j];
      }
      delete clusters_[i];
    }
  }

  /**
   * Add a document
   *
   * @param doc document object
   * @return void
   */
  void add_document(Document &doc) {
    Document *ptr = new Document(doc.id(), doc.feature());
    doc.set_feature(NULL);
    documents_.push_back(ptr);
  }

  /**
   * Do clustering
   *
   * @param mode clustering mode(rb, kmeans)
   * @return size_t number of clusters
   */
  size_t do_clustering(const std::string &mode);

  /**
   * Get next clustering result
   *
   * @param cluster output cluster
   * @return bool  return true if next result exists
   */
  bool get_next_result(Cluster &cluster) {
    if (cluster_index_ < clusters_.size()) {
      cluster = *clusters_[cluster_index_++];
      return true;
    }
    return false;
  }

  /**
   * Set condition of size of clusters
   *
   * @param nclusters size of clusters
   * @return void
   */
  void set_cluster_size_limit(size_t nclusters) {
    limit_nclusters_ = nclusters;
  }

  /**
   * Set condition of point of sectioned gain
   *
   * @param limit  sectioned gain
   * @return void
   */
  void set_eval_limit(double limit) {
    limit_eval_ = limit;
  }
};

} /* namespace bayon */

#endif
