//
// Utility class for vector operation
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

#ifndef BAYON_BYVECTOR_H_
#define BAYON_BYVECTOR_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <utility>
#include <vector>
#include "util.h"

namespace bayon {

typedef long                            VecKey;    ///< key of a vector
typedef double                          VecValue;  ///< value of a vector
typedef std::pair<VecKey, VecValue>     VecItem;   ///< key-value pair
typedef HashMap<VecKey, VecValue>::type VecHashMap;

const VecKey   VECTOR_EMPTY_KEY   = -1;   ///< empty key for google::hash_map
const VecKey   VECTOR_DELETED_KEY = -2;   ///< deleted key for google::hash_map
const VecValue VECTOR_NULL_VALUE  = 0.0;  ///< value of nonexistent entry


/**
 * Vector class.
 * This is a utility class for vector operations.
 */
class Vector {
 private:
  VecHashMap vec_;  ///< Internal hash_map object

 public:
  /**
   * Constructor.
   */
  Vector() {
    init_hash_map(VECTOR_EMPTY_KEY, vec_);
  }

  /**
   * Destructor.
   */
  ~Vector() { }

  /**
   * Set a bucket count of the internal hash_map object.
   * @param n bucket count
   */
  void set_bucket_count(size_t n) {
#if defined(HAVE_GOOGLE_DENSE_HASH_MAP) || defined(HAVE_EXT_HASH_MAP)
    vec_.rehash(n);
#endif
  }

  /**
   * Set max_load_factor of the internal hash_map object.
   * @param factor max_load_factor value
   */
  void max_load_factor(double factor) {
    vec_.max_load_factor(factor);
  }

  /**
   * Copy data to a vector.
   * @param vec output vector
   */
  void copy(Vector &vec) const {
    vec.clear();
    vec.set_bucket_count(vec_.size());
    for (VecHashMap::const_iterator it = vec_.begin();
         it != vec_.end(); ++it) {
      vec.set(it->first, it->second);
    }
  }

  /**
   * Get a value.
   * @param key key
   * @return value
   */
  VecValue get(VecKey key) const {
    VecHashMap::const_iterator it = vec_.find(key);
    return (it != vec_.end()) ? it->second : VECTOR_NULL_VALUE;
  }

  /**
   * Set a value.
   * @param key key
   * @param value value
   */
  void set(VecKey key, VecValue value) {
    vec_[key] = value;
  }

  /**
   * Get the size of a vector.
   * @return the size of a vector
   */
  size_t size() const {
    return vec_.size();
  }

  /**
   * Clear all items.
   */
  void clear() {
    vec_.clear();
    vec_.max_load_factor(0.9);
  }

  /**
   * Get the const pointer of a internal hash_map object.
   * @return the pointer of a internal hash_map object
   */
  const VecHashMap *hash_map() const {
    return &vec_;
  }

  /**
   * Get the pointer of a internal hash_map object.
   * @return the pointer of hash_map object
   */
  VecHashMap *hash_map() {
    return &vec_;
  }

  /**
   * Get items sorted by values (desc order).
   * @param items sorted keys
   */
  void sorted_items(std::vector<VecItem> &items) const;

  /**
   * Get items sorted by absolute values (desc order).
   * @param items sorted keys
   */
  void sorted_items_abs(std::vector<VecItem> &items) const;

  /**
   * Normalize a vector.
   */
  void normalize();

  /**
   * Resize a vector.
   * @param size resized size
   */
  void resize(size_t size);

  /**
   * Calculate a squared norm.
   * @return a squared norm
   */
  double norm_squared() const;

  /**
   * Calculate a norm.
   * @return norm
   */
  double norm() const;

  /**
   * Multiply each value of a vector by a constant value.
   * @param x a constant value
   */
  void multiply_constant(double x);

  /**
   * Add other vector.
   * @param vec an input vector
   */
  void add_vector(const Vector &vec);

  /**
   * Delete other vector.
   * @param vec an input vector
   */
  void delete_vector(const Vector &vec);

  /**
   * Calculate the squared euclid distance between vectors.
   * @param vec1 an input vector
   * @param vec2 an input vector
   * @return squared distance
   */
  static double euclid_distance_squared(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate the euclid distance bewteen vectors.
   * @param vec1 input vector
   * @param vec2 input vector
   * @return distance
   */
  static double euclid_distance(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate the inner product value between vectors.
   * @param vec1 input vector
   * @param vec2 input vector
   * @return inner product value
   */
  static double inner_product(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate the cosine value between vectors.
   * @param vec1 input vector
   * @param vec2 input vector
   * @return cosine value
   */
  static double cosine(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate the Jaccard coefficient value between vectors.
   * @param vec1 input vector
   * @param vec2 input vector
   * @return jaccard coefficient value
   */
  static double jaccard(const Vector &vec1, const Vector &vec2);

  /**
   * Output stream.
   * @param ofs output stream
   * @param vec a vector object
   */
  friend std::ostream &operator <<(std::ostream &os, const Vector &vec) {
    os.precision(4);
    for (VecHashMap::const_iterator it = vec.vec_.begin();
         it != vec.vec_.end(); ++it) {
      if (it != vec.vec_.begin()) os << DELIMITER;
      os << it->first << DELIMITER << it->second;
    }
    return os;
  }

  /**
   * Print a vector.
   */
  void print() const {
    for (VecHashMap::const_iterator it = vec_.begin();
         it != vec_.end(); ++it) {
      if (it != vec_.begin()) printf("%s", DELIMITER.c_str());
      printf("%ld%s%.4f", it->first, DELIMITER.c_str(), it->second);
    }
    printf("\n");
  }
};

} /* namespace bayon */

#endif  // BAYON_BYVECTOR_H_
