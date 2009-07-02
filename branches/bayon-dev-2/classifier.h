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
 * Classes
 *******************************************************************/
/**
 * Classifier class
 */
class Classifier {
 private:
  HashMap<VectorId, Vector>::type vectors_;

 public:
  /**
   * Constructor
   */
  Classifier() {
    init_hash_map(EMPTY_KEY, vectors_);  
  }

  /**
   * Destructor
   */
  ~Classifier() { }

  /**
   * Add vector
   *
   * @param id vector id
   * @param vec Vector object
   * @return void
   */
  void add_vector(VectorId id, const Vector &vec) {
    vectors_[id] = vec;
    vectors_[id].normalize();
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
   * Get list of id and points of similar vectors
   *
   * @param vec Vector object (must be normalized)
   * @param items pairs of id and similarity point
   * @return void
   */
  void similar_vectors(
    const Vector &vec,
    std::vector<std::pair<VectorId, double> > &items) const;
};

} /* namespace bayon */

#endif
