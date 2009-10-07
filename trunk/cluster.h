//
// Clustering API
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

#ifndef BAYON_CLUSTER_H
#define BAYON_CLUSTER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>
#include "byvector.h"
#include "util.h"

namespace bayon {

/********************************************************************
 * Typedef
 *******************************************************************/
/* Document ID */
typedef long DocumentId;


/********************************************************************
 * Constants
 *******************************************************************/
const DocumentId DOC_EMPTY_KEY   = -1;


/*********************************************************************
 * Classes
 ********************************************************************/
/**
 * Document class
 */
class Document {
 private:
  DocumentId id_;
  Vector *feature_;

 public:
  /**
   * Constructor
   *
   * @param id document id
   */
  Document(DocumentId id) : id_(id) {
    feature_ = new Vector;
  }

  /**
   * Constructor
   *
   * @param id document id
   * @param feature features of document
   */
  Document(DocumentId id, Vector *feature) : id_(id), feature_(feature) { }

  /**
   * Destructor
   *
   * if feature_ is null, delete it
   */
  ~Document() {
    if (feature_) delete feature_;
  }

  /**
   * Get document id
   *
   * @return DocumentId  document id
   */
  DocumentId id() const {
    return id_;
  }

  /**
   * Get feature vector
   *
   * @return Vector *  feature vector
   */
  Vector *feature() {
    return feature_;
  }

  /**
   * Get feature vector
   *
   * @return const Vector *  feature vector
   */
  const Vector *feature() const {
    return feature_;
  }

  /**
   * Add feature of document
   *
   * @param key key of feature
   * @param value value of feature
   * @return void
   */
  void add_feature(VecKey key, VecValue value) {
    feature_->set(key, value);
  }

  /**
   * Set features
   *
   * @param feature Vector object
   * @return void
   */
  void set_features(Vector *feature) {
    feature_ = feature;
  }

  /**
   * Clear features
   *
   * @return void
   */
  void clear() {
    feature_->clear();
  }

};


/**
 * Cluster class
 */
class Cluster {
 private:
  /**
   * documents in the cluster
   */
  std::vector<Document *> documents_;

  /**
   * composite vector
   */
  Vector composite_;

  /**
   * centroid vector
   */
  Vector centroid_;

  /**
   * removed documents
   */
  HashMap<DocumentId, bool>::type removed_;

  /**
   * sectioned clusters
   */
  std::vector<Cluster *> sectioned_clusters_;

  /**
   * sectioned gain
   */
  double sectioned_gain_;

  /**
   * seed for random number generator
   */
  unsigned int seed_;

  /**
   * add the vectors of all documents to composite vector
   */
  void set_composite_vector() {
    composite_.clear();
    for (size_t i = 0; i < documents_.size(); i++) {
      composite_.add_vector(*documents_[i]->feature());
    }
  }

 public:
  Cluster() : sectioned_gain_(0), seed_(DEFAULT_SEED) {
    init_hash_map(DOC_EMPTY_KEY, removed_);
  }

  Cluster(size_t n) : sectioned_gain_(0), seed_(DEFAULT_SEED) {
    init_hash_map(DOC_EMPTY_KEY, removed_);
    composite_.set_bucket_count(n);
//    centroid_.set_bucket_count(n);
//    removed_.resize(n);
  }

  ~Cluster() { }

  /**
   * Clear
   *
   * @return void
   */
  void clear() {
    documents_.clear();
    composite_.clear();
    centroid_.clear();
    removed_.clear();
    sectioned_clusters_.clear();
    sectioned_gain_ = 0.0;
  }

  /**
   * Set seed for random number generator
   *
   * @param seed seed
   * @return void
   */
  void set_seed(unsigned int seed) {
    seed_ = seed;
    mysrand(seed_);
  }

  /**
   * Get size of the cluster
   *
   * @return size_t size of cluster
   */
  size_t size() const {
    return documents_.size() - removed_.size();
  }

  /**
   * Get centroid Vector of the cluster
   *
   * @return Vector * centroid vector
   */
  Vector *centroid_vector() {
    if (documents_.size() > 0 && composite_.size() == 0) {
      set_composite_vector();
    }
    composite_vector()->copy(centroid_);
    centroid_.normalize();
    return &centroid_;
  }

  /**
   * Get composite Vector of the cluster
   *
   * @return Vector * composite vector
   */
  Vector *composite_vector() {
    return &composite_;
  }

  /**
   * Get documents
   *
   * @return const std::vector<Document *> &  list of documents in the cluster
   */
  const std::vector<Document *> &documents() const  {
    return documents_;
  }

  /**
   * Add a document
   *
   * @param doc pointer of document object
   * @return void
   */
  void add_document(Document *doc) {
    doc->feature()->normalize();
    documents_.push_back(doc);
    composite_.add_vector(*doc->feature());
  }

  /**
   * Remove a document
   *
   * @param index  index of vector container of documents
   * @return void
   */
  void remove_document(size_t index) {
    composite_.delete_vector(*documents_[index]->feature());
    removed_[documents_[index]->id()] = true;
  }

  /**
   * Remove a document
   *
   * @param doc  pointer of document object
   * @return void
   */
  void remove_document(const Document *doc) {
    for (size_t i = 0; i < documents_.size(); i++) {
      if (documents_[i]->id() == doc->id()) {
        remove_document(i);
        return;
      }
    }
  }

  /**
   * Get sorted documents in clusters by similarity
   * between documents and center (desc order)
   *
   * @param pairs pairs of document and similarity point
   * @return void
   */
  void sorted_documents(std::vector<std::pair<Document *, double> > &pairs);

  /**
   * delete removed documents from internal container
   *
   * @return void
   */
  void refresh() {
    if (removed_.size() > 0) {
      std::vector<Document *> docs;
      for (size_t i = 0; i < documents_.size(); i++) {
        if (removed_.find(documents_[i]->id()) == removed_.end()) {
          docs.push_back(documents_[i]);
        }
      }
      documents_ = docs;
      removed_.clear();
    }
  }

  /**
   * Get gain when section the cluster
   *
   * @return dobule gain
   */
  double sectioned_gain() const {
    return sectioned_gain_;
  }

  /**
   * Set gain when the cluster was sectioned
   *
   * @return void
   */
  void set_sectioned_gain();

  /**
   * Get sectioned clusters
   *
   * @return std::vector<Cluster *> list of clusters
   */
  std::vector<Cluster *> &sectioned_clusters() {
    return sectioned_clusters_;
  }

  const std::vector<Cluster *> &sectioned_clusters() const {
    return sectioned_clusters_;
  }

  /**
   * Choose documents randomly
   *
   * @param ndocs number of documents
   * @param docs documents
   * @return void
   */
  void choose_randomly(size_t ndocs, std::vector<Document *> &docs);

  /**
   * Choose documents smartly
   *
   * @param ndocs number of documents
   * @param docs documents
   * @return void
   */
  void choose_smartly(size_t ndocs, std::vector<Document *> &docs);

  /**
   * Section the cluster
   *
   * @param nclusters number of clusters
   * @param clusters output clusters
   * @return void
   */
  void section(size_t nclusters);

  /**
   * output stream
   */
  friend std::ostream & operator <<(std::ostream &os, const Cluster &cluster) {
    for (size_t i = 0; i < cluster.documents_.size(); i++) {
      if (i > 0) os << DELIMITER;
      os << cluster.documents_[i]->id();
    }
    return os;
  }
};


/**
 * Analyzer class
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
   * seed for random number generator
   */
  unsigned int seed_;

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

  /**
   * Count document frequency(DF) of vector keys
   *
   * @param df document frequency
   * @return void
   */
  void count_df(HashMap<VecKey, size_t>::type &df) const;

 public:
  /**
   * Constructor
   */
  Analyzer() : cluster_index_(0), limit_nclusters_(0), limit_eval_(-1.0),
               seed_(DEFAULT_SEED) { }

 /**
  * Constructor
  *
  * @param seed seed for random number generator
  */
  Analyzer(unsigned int seed) : cluster_index_(0), limit_nclusters_(0),
                                limit_eval_(-1.0), seed_(seed) { }

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
   * Set seed for random number generator
   *
   * @param seed seed
   * @return void
   */
  void set_seed(unsigned int seed) {
    seed_ = seed;
  }

  /**
   * Add a document
   *
   * @param doc document object
   * @return void
   */
  void add_document(Document &doc) {
    Document *ptr = new Document(doc.id(), doc.feature());
    doc.set_features(NULL);
    documents_.push_back(ptr);
  }

  /**
   * Get documents
   *
   * @return std::vector<Document *> & documents
   */
  std::vector<Document *> &documents() {
    return documents_;
  }

  /**
   * Resize size of feature vectors of documents
   *
   * @param siz size of feature vectors
   * @return void
   */
  void resize_document_features(size_t siz) {
    for (size_t i = 0; i < documents_.size(); i++) {
      documents_[i]->feature()->resize(siz);
    }
  }

  /**
   * Get clusters
   *
   * @return std::vector<Cluster *> & clusters
   */
  std::vector<Cluster *> &clusters() {
    return clusters_;
  }

  /**
   * Calculate inverse document frequency(IDF)
   * and apply it to document vectors
   */
  void idf();

  /**
   * Calculate standard socre
   * and apply it to document vectors
   */
  void standard_score();

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

  void cluster_similarities(Document * document,
    std::vector<std::pair<size_t, double> > &similarities);
};


/********************************************************************
 * Functions
 *******************************************************************/
/**
 * Compare clusters by size of cluster
 *
 * @param a  cluster
 * @param b  cluster
 * @return bool  if a.size() < b.size(), return true
 */
struct CompareClusterSizeGreater {
  bool operator() (const Cluster *a, const Cluster *b) {
    return a->size() < b->size();
  }
};

/**
 * Compare clusters by evaluation of bisection
 *
 * @param a  cluster
 * @param b  cluster
 * @return bool  if a.sectioned_gain() < b.sectioned_gain(), return true
 */
struct CompareClusterBisectionEvalGreater {
  bool operator() (const Cluster *a, const Cluster *b) {
    return a->sectioned_gain() < b->sectioned_gain();
  }
};

} /* namespace bayon */

#endif
