//
// Classifier class
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

#ifndef BAYON_CLASSIFIER_H_
#define BAYON_CLASSIFIER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <utility>
#include <vector>
#include "byvector.h"

namespace bayon {

/**
 * Classifier class.
 */
class Classifier {
 public:
  /** the identifier of a vector */
  typedef long VectorId;
  /** the items in inverted indexes */
  typedef std::pair<VectorId, double> IndexItem;
  /** the value of inverted indexes */
  typedef std::vector<IndexItem> InvertedIndexValue;
  /** inverted index */
  typedef HashMap<VecKey, InvertedIndexValue *>::type InvertedIndex;

 private:
  static const VectorId VECID_EMPTY_KEY = -1;  ///< empty key

  HashMap<VectorId, Vector>::type vectors_;  ///< input vectors
  InvertedIndex inverted_index_;             ///< inverted index

  /**
   * Add vector keys to inverted index.
   * @param id the identifier of a vector
   * @param vec a feature vector
   */
  void update_inverted_index(VectorId id, const Vector &vec);

  /**
   * Look up inverted index.
   * @param max the maximum number of keys of each vector
   *            to be looked up in inverted index
   * @param vec input vectors
   * @param ids list of VectorId
   * @return the number of output VectorId
   */
  size_t lookup_inverted_index(size_t max, const Vector &vec,
                               std::vector<VectorId> &ids) const;

 public:
  /**
   * Constructor.
   */
  Classifier() {
    init_hash_map(VECID_EMPTY_KEY, vectors_);
    init_hash_map(VECID_EMPTY_KEY, inverted_index_);
  }

  /**
   * Destructor.
   */
  ~Classifier() {
    for (InvertedIndex::iterator it = inverted_index_.begin();
         it != inverted_index_.end(); ++it) {
      if (it->second) delete it->second;
    }
  }

  /**
   * Add a vector.
   * @param id the identifier of a vector
   * @param vec a feature vector
   */
  void add_vector(VectorId id, const Vector &vec) {
    vectors_[id] = vec;
    vectors_[id].normalize();
    update_inverted_index(id, vec);
  }

  /**
   * Get the number of vectors.
   * @return the number of vectors
   */
  size_t count_vectors() const {
    return vectors_.size();
  }

  /**
   * Resize a inverted index.
   * @param the size of resized index
   */
  void resize_inverted_index(size_t siz);

  /**
   * Get the pairs of the identifiers and points of similar vectors.
   * @param max the maximum number of keys of each vector
   *            to be looked up in inverted index
   * @param vec a feature vector (must be normalized)
   * @param items pairs of the identifiers and similarity points
   */
  void similar_vectors(
    size_t max, const Vector &vec,
    std::vector<std::pair<VectorId, double> > &items) const;
};

} /* namespace bayon */

#endif  // BAYON_CLASSIFIER_H_
