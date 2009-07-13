//
// Classifier
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

#ifndef BAYON_CLASSIFIER_H
#define BAYON_CLASSIFIER_H

#include <utility>
#include <vector>
#include "byvector.h"

namespace bayon {

/********************************************************************
 * Typedef
 *******************************************************************/
/* Vector ID */
typedef long VectorId;


/********************************************************************
 * Constants
 *******************************************************************/
const VectorId VECID_EMPTY_KEY = -1;


/********************************************************************
 * Classes
 *******************************************************************/
/**
 * Classifier class
 */
class Classifier {
 public:
  typedef std::pair<VectorId, double> IndexItem;
  typedef std::vector<IndexItem> InvertedIndexValue;
  typedef HashMap<VecKey, InvertedIndexValue *>::type InvertedIndex;

 private:
  /* input vectors */
  HashMap<VectorId, Vector>::type vectors_;

  /* inverted index */
  InvertedIndex inverted_index_;

  /**
   * Add vector keys to inverted index
   *
   * @param id vector id
   * @param vec Vector object
   * @return void
   */
  void add_inverted_index(VectorId id, const Vector &vec);

  /**
   * Look up inverted index
   *
   * @param max max size of keys of each vector to be
   *            looked up in inverted index
   * @param vec input vectors
   * @param ids list of VectorId
   * @return size_t the number of output VectorId
   */
  size_t lookup_inverted_index(size_t max, const Vector &vec,
                               std::vector<VectorId> &ids) const;

 public:
  /**
   * Constructor
   */
  Classifier() {
    init_hash_map(VECID_EMPTY_KEY, vectors_);  
    init_hash_map(VECID_EMPTY_KEY, inverted_index_);  
  }

  /**
   * Destructor
   */
  ~Classifier() {
    for (InvertedIndex::iterator it = inverted_index_.begin();
         it != inverted_index_.end(); ++it) {
      if (it->second) delete it->second;    
    }
  }

  /**
   * Add vector
   *
   * @param id vector id
   * @param vec Vector object
   */
  void add_vector(VectorId id, const Vector &vec) {
    vectors_[id] = vec;
    vectors_[id].normalize();
    add_inverted_index(id, vec);
  }

  /**
   * Get the number of vectors
   *
   * @return size_t number of vectors
   */
  size_t count_vectors() {
    return vectors_.size();
  }

  /**
   * Resize inverted index
   *
   * @param siz size of resized index
   */
  void resize_inverted_index(size_t siz);

  /**
   * Get list of id and points of similar vectors
   *
   * @param max max size of keys of each vector to be
   *            looked up in inverted index
   * @param vec Vector object (must be normalized)
   * @param items pairs of id and similarity point
   */
  void similar_vectors(
    size_t max, const Vector &vec,
    std::vector<std::pair<VectorId, double> > &items) const;
};

} /* namespace bayon */

#endif
