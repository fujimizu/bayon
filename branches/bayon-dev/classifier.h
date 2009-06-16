//
// Document Classifier
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
#include "cluster.h"

namespace bayon {

/* typedef */
typedef uint32_t ClusterId;  // Cluster ID

/*********************************************************************
 * Classifier class
 ********************************************************************/
class Classifier {
 private:
  HashMap<ClusterId, Vector>::type vectors_;

 public:
  /**
   * Get list of id and points of similar vectors
   *
   * @param document document object
   * @param items pairs of id and similarity point
   * @return void
   */
  void similar_vectors(
    const Document &document,
    std::vector<std::pair<ClusterId, double> > &items) const;

  /**
   * Add vector
   *
   * @param id cluster id
   * @param vec Vector object
   * @return void
   */
  void add_vector(ClusterId id, const Vector &vec) {
    vectors_[id] = vec;
  }

};

} /* namespace bayon */

#endif
