//
// Clustering API
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

#ifndef BAYON_CLUSTER_H
#define BAYON_CLUSTER_H

#include <algorithm>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>
#include "config.h"
#include "clvector.h"
#include "util.h"

namespace bayon {

/* typedef */
typedef int32_t DocumentId;  // Document ID
//typedef int64_t DocumentId;  // Document ID

/* constants */
const DocumentId DOCUMENT_ID_EMPTY = -1;


/*********************************************************************
 * Document class
 ********************************************************************/
class Document {
 private:
  DocumentId id_;
  Vector *feature_;

 public:
  Document(DocumentId id) : id_(id) {
    feature_ = new Vector;
  }

  Document(DocumentId id, Vector *feature) : id_(id), feature_(feature) { }

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
   * Set feature
   *
   * @param feature Vector object
   * @return void
   */
  void set_feature(Vector *feature) {
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


/*********************************************************************
 * Cluster class
 ********************************************************************/
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
   * Sectioned clusters
   */
  std::vector<Cluster *> sectioned_clusters_;

  /**
   * sectioned gain
   */
  double sectioned_gain_;

 public:
  Cluster() : sectioned_gain_(0) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP 
    removed_.set_empty_key(DOCUMENT_ID_EMPTY);
#endif
  }

  Cluster(size_t n) : sectioned_gain_(0) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP 
    removed_.set_empty_key(DOCUMENT_ID_EMPTY);
#endif
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
    composite_.copy(centroid_);
    centroid_.normalize();
    return &centroid_;
  }

  /**
   * Get composite Vector of the cluster
   * @return Vector * composite vector
   */
  Vector *composite_vector() {
    return &composite_;
  }

  const Vector *composite_vector() const {
    return &composite_;
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
   */
  void remove_document(size_t index) {
    composite_.delete_vector(*documents_[index]->feature());
    removed_[documents_[index]->id()] = true;
  }

  /**
   * delete removed documents from internal container
   *
   * @return void
   */
  void refresh() {
    if (removed_.size() > 0) {
      std::vector<Document *> docs;
      for (size_t i = 0; i < documents_.size(); i++) {
        if (!removed_[documents_[i]->id()]) {
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
   * Get documents
   *
   * @return std::vector<Document *> &  list of documents in the cluster
   */
  const std::vector<Document *> &documents() const  {
    return documents_;
  }

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
  void choose_randomly(size_t ndocs, std::vector<Document *> &docs) const;

  /**
   * Choose documents smartly
   *
   * @param ndocs number of documents
   * @param docs documents
   * @return void
   */
  void choose_smartly(size_t ndocs, std::vector<Document *> &docs) const;

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
  friend std::ostream & operator <<(std::ostream &os, Cluster &cluster) {
    for (size_t i = 0; i < cluster.documents_.size(); i++) {
      if (i > 0) os << " ";
      os << cluster.documents_[i]->id();
    }
    return os;
  }
};


/*********************************************************************
 * Analyzer class
 ********************************************************************/
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
  Analyzer() : cluster_index_(0), limit_nclusters_(0), limit_eval_(-1.0) { }

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
   */
  void add_document(Document &doc) {
    Document *ptr = new Document(doc.id(), doc.feature());
    doc.set_feature(NULL);
    documents_.push_back(ptr);
  }

  /**
   * Do clustering
   *
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
