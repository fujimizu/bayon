//
// Cluster class
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

#ifndef BAYON_CLUSTER_H_
#define BAYON_CLUSTER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <utility>
#include <vector>
#include "document.h"

namespace bayon {

/**
 * Cluster class.
 */
class Cluster {
 private:
  std::vector<Document *> documents_;          ///< documents
  Vector composite_;                           ///< a composite vector
  Vector centroid_;                            ///< a centroid vector
  HashMap<DocumentId, bool>::type removed_;    ///< removed documents
  std::vector<Cluster *> sectioned_clusters_;  ///< sectioned clusters
  double sectioned_gain_;                      ///< a sectioned gain
  unsigned int seed_;                          ///< seed

  /**
   * Add the vectors of all documents to a composite vector.
   */
  void set_composite_vector() {
    composite_.clear();
    composite_.set_bucket_count(size() * 100);
    for (size_t i = 0; i < documents_.size(); i++) {
      composite_.add_vector(*documents_[i]->feature());
    }
  }

 public:
  /**
   * Constructor.
   */
  Cluster() : sectioned_gain_(0), seed_(DEFAULT_SEED) {
    init_hash_map(DOC_EMPTY_KEY, removed_);
  }

  /**
   * Constructor.
   * @param n the bucket count of a composite vector
   */
  Cluster(size_t n) : sectioned_gain_(0), seed_(DEFAULT_SEED) {
    init_hash_map(DOC_EMPTY_KEY, removed_);
    composite_.set_bucket_count(n);
    centroid_.set_bucket_count(n);
    // removed_.resize(n);
  }

  /**
   * Destructor.
   */
  ~Cluster() { }

  /**
   * Clear status.
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
   * Set a seed value for a random number generator.
   * @param seed a seed value
   */
  void set_seed(unsigned int seed) {
    seed_ = seed;
    mysrand(seed_);
  }

  /**
   * Get the size.
   * @return the size of this cluster
   */
  size_t size() const {
    return documents_.size() - removed_.size();
  }

  /**
   * Get the pointer of a centroid vector.
   * @return the pointer of a centroid vector
   */
  Vector *centroid_vector() {
    if (documents_.size() > 0 && !composite_.size()) set_composite_vector();
    composite_vector()->copy(centroid_);
    centroid_.normalize();
    return &centroid_;
  }

  /**
   * Get the pointer of a composite vector.
   * @return the pointer of a composite vector
   */
  Vector *composite_vector() {
    return &composite_;
  }

  /**
   * Get documents in this cluster.
   * @return documents in this cluster
   */
  const std::vector<Document *> &documents() const  {
    return documents_;
  }

  /**
   * Add a document.
   * @param doc the pointer of a document object
   */
  void add_document(Document *doc) {
    doc->feature()->normalize();
    documents_.push_back(doc);
    composite_.add_vector(*doc->feature());
  }

  /**
   * Remove a document from this cluster.
   * @param index the index of vector container of documents
   */
  void remove_document(size_t index) {
    composite_.delete_vector(*documents_[index]->feature());
    removed_[documents_[index]->id()] = true;
  }

  /**
   * Remove a document from this cluster.
   * @param doc the pointer of a document object
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
   * Get sorted documents in this clusters.
   * by similarity between documents and center (desc order)
   * @param pairs pairs of documents and similarity points
   */
  void sorted_documents(std::vector<std::pair<Document *, double> > &pairs);

  /**
   * Delete removed documents from the internal container.
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
   * Get a gain when this cluster sectioned.
   * @return a gain
   */
  double sectioned_gain() const {
    return sectioned_gain_;
  }

  /**
   * Set a gain when the cluster sectioned.
   */
  void set_sectioned_gain();

  /**
   * Get sectioned clusters.
   * @return sectioned clusters
   */
  std::vector<Cluster *> &sectioned_clusters() {
    return sectioned_clusters_;
  }

  /**
   * Get sectioned clusters.
   * @return sectioned clusters
   */
  const std::vector<Cluster *> &sectioned_clusters() const {
    return sectioned_clusters_;
  }

  /**
   * Choose documents randomly.
   * @param ndocs number of documents
   * @param docs documents
   */
  void choose_randomly(size_t ndocs, std::vector<Document *> &docs);

  /**
   * Choose documents smartly.
   * @param ndocs number of documents
   * @param docs documents
   */
  void choose_smartly(size_t ndocs, std::vector<Document *> &docs);

  /**
   * Section this cluster.
   * @param nclusters number of clusters
   * @param clusters output clusters
   */
  void section(size_t nclusters);

  /**
   * Output stream.
   * @param ofs output stream
   * @cluster a cluster
   */
  friend std::ostream & operator <<(std::ostream &os, const Cluster &cluster) {
    for (size_t i = 0; i < cluster.documents_.size(); i++) {
      if (i > 0) os << DELIMITER;
      os << cluster.documents_[i]->id();
    }
    return os;
  }

  /**
   * Print this cluster
   */
  void print() {
    for (size_t i = 0; i < documents_.size(); i++) {
      if (i > 0) printf("%s", DELIMITER.c_str());
      printf("%ld", documents_[i]->id());
    }
    printf("\n");
  }
};

/**
 * Compare clusters by sizes of clusters.
 * @param a  cluster
 * @param b  cluster
 * @return if a.size() < b.size(), return true
 */
struct CompareClusterSizeGreater {
  bool operator() (const Cluster *a, const Cluster *b) {
    return a->size() < b->size();
  }
};

/**
 * Compare clusters by evaluation of bisection.
 * @param a  cluster
 * @param b  cluster
 * @return if a.sectioned_gain() < b.sectioned_gain(), return true
 */
struct CompareClusterBisectionEvalGreater {
  bool operator() (const Cluster *a, const Cluster *b) {
    return a->sectioned_gain() < b->sectioned_gain();
  }
};

}  /* namespace bayon */

#endif  // BAYON_CLUSTER_H_
