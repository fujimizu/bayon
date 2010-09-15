//
// Cluster analyzer class
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

#ifndef BAYON_ANALYZER_H_
#define BAYON_ANALYZER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include "byvector.h"
#include "cluster.h"
#include "document.h"

namespace bayon {

/**
 * Analyzer class
 */
class Analyzer {
 public:
  /**
   * clustering methods
   */
  enum Method {
    RB,     ///< repeated bisection
    KMEANS  ///< kmeans
  };

 private:
  /** maximum count of cluster refinement loop */
  static const unsigned int NUM_REFINE_LOOP = 30;

  std::vector<Document *> documents_;  ///< documents
  std::vector<Cluster *> clusters_;    ///< clustering results
  size_t cluster_index_;               ///< the index of clusters
  size_t limit_nclusters_;             ///< maximum number of clusters
  double limit_eval_;                  ///< limit of sectioned points
  unsigned int seed_;                  ///< a seed of a random number generator

  /**
   * Do repeated bisection clustering.
   * @return the number of clusters
   */
  size_t repeated_bisection();

  /**
   * Do k-means clustering.
   * @return the number of clusters
   */
  size_t kmeans();

  /**
   * Refine clustering results.
   * @param clusters clusters to be refined
   * @return the value of refiend clusters
   */
  double refine_clusters(std::vector<Cluster *> &clusters);

  inline double refined_vector_value(const Vector &composite,
                                     const Vector &vec, int sign);

  /**
   * Count document frequency(DF) of the features in documents.
   * @param df document frequency
   */
  void count_df(HashMap<VecKey, size_t>::type &df) const;

 public:
  /**
   * Constructor.
   */
  Analyzer() : cluster_index_(0), limit_nclusters_(0), limit_eval_(-1.0),
               seed_(DEFAULT_SEED) { }

 /**
  * Constructor.
  * @param seed seed for random number generator
  */
  explicit Analyzer(unsigned int seed)
    : cluster_index_(0), limit_nclusters_(0), limit_eval_(-1.0), seed_(seed) { }

 /**
  * Destructor.
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
   * Set a seed value for a random number generator.
   * @param seed a seed value
   */
  void set_seed(unsigned int seed) {
    seed_ = seed;
  }

  /**
   * Add a document.
   * @param doc a document object
   */
  void add_document(Document &doc) {
    Document *ptr = new Document(doc.id(), doc.feature());
    doc.set_features(NULL);
    documents_.push_back(ptr);
  }

  /**
   * Get documents.
   * @return documents
   */
  std::vector<Document *> &documents() {
    return documents_;
  }

  /**
   * Resize the feature vectors of documents.
   * @param siz resized sizes of feature vectors
   */
  void resize_document_features(size_t siz) {
    for (size_t i = 0; i < documents_.size(); i++) {
      documents_[i]->feature()->resize(siz);
    }
  }

  /**
   * Get clusters.
   * @return clusters
   */
  std::vector<Cluster *> &clusters() {
    return clusters_;
  }

  /**
   * Calculate inverse document frequency(IDF)
   * and apply it to document vectors.
   */
  void idf();

  /**
   * Calculate standard socre and apply it to document vectors.
   */
  void standard_score();

  /**
   * Do clustering.
   * @param method clustering method
   * @return the number of clusters
   */
  size_t do_clustering(Method method);

  /**
   * Get the next clustering result.
   * @param cluster output cluster
   * @return return true if next result exists
   */
  bool get_next_result(Cluster &cluster) {
    if (cluster_index_ < clusters_.size()) {
      cluster = *clusters_[cluster_index_++];
      return true;
    }
    return false;
  }

  /**
   * Set the maximum number of clusters.
   * @param nclusters the maximum number of clusters
   */
  void set_cluster_size_limit(size_t nclusters) {
    limit_nclusters_ = nclusters;
  }

  /**
   * Set the minumum value of sectioned gain.
   * @param limit the minimum value of sectioned gain
   */
  void set_eval_limit(double limit) {
    limit_eval_ = limit;
  }
};

}  /* namespace bayon */

#endif  // BAYON_ANALYZER_H_
