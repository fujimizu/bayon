//
// Cluster class
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
#include "byvector.h"
#include "config.h"
#include "document.h"
#include "util.h"

namespace bayon {

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
   * Sectioned clusters
   */
  std::vector<Cluster *> sectioned_clusters_;

  /**
   * Pointer of random generator
   */
  Random *random_;

  /**
   * sectioned gain
   */
  double sectioned_gain_;

 public:
  Cluster() : sectioned_gain_(0) {
    init_hash_map(EMPTY_KEY, DELETED_KEY, removed_);
  }

  Cluster(size_t n) : sectioned_gain_(0) {
    init_hash_map(EMPTY_KEY, DELETED_KEY, removed_);
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
    if (documents_.size() > 0 && composite_.size() == 0) {
      for (size_t i = 0; i < documents_.size(); i++) {
        composite_.add_vector(*documents_[i]->feature());
      }
    }
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
   * Get documents
   *
   * @return std::vector<Document *> &  list of documents in the cluster
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
   */
  void remove_document(size_t index) {
    composite_.delete_vector(*documents_[index]->feature());
    removed_[documents_[index]->id()] = true;
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

  void set_random_generator(Random *r) {
    random_ = r;
  }

  /**
   * output stream
   */
  friend std::ostream & operator <<(std::ostream &os, Cluster &cluster) {
    for (size_t i = 0; i < cluster.documents_.size(); i++) {
      if (i > 0) os << "\t";
      os << cluster.documents_[i]->id();
    }
    return os;
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
