//
// Utilities for vector operation
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

#ifndef BAYON_CLVECTOR_H
#define BAYON_CLVECTOR_H

#include <cmath>
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "config.h"
#include "util.h"

namespace bayon {

/* typedef */
typedef int32_t                         VecKey;   // key of vector
//typedef int64_t                         VecKey;   // key of vector
typedef double                          VecValue; // value of vector
typedef std::pair<VecKey, VecValue>     VecItem;  // key-value pair
typedef HashMap<VecKey, VecValue>::type VecHashMap;

/* Constants */
const VecKey   VECTOR_EMPTY_KEY   = -1;
const VecKey   VECTOR_DELETED_KEY = -2;
const VecValue VECTOR_NULL_VALUE  = 0.0;


/*********************************************************************
 * Vector class
 ********************************************************************/
class Vector {
 private:
  VecHashMap vec_;

  /**
   * initialize internal hash_map object
   * for google::dense_hash_map
   *
   * @return void
   */
  void init_hash_map() {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
    vec_.max_load_factor(0.9);
    vec_.set_empty_key(VECTOR_EMPTY_KEY);
    vec_.set_deleted_key(VECTOR_DELETED_KEY);
#endif
  }

 public:
  Vector() {
    init_hash_map();
  }

  Vector(const Vector &vec) {
    init_hash_map();
    for (VecHashMap::const_iterator it = vec.hash_map()->begin();
         it != vec.hash_map()->end(); ++it) {
      vec_[it->first] = it->second;
    }
  }

  ~Vector() { }

  /**
   * Set bucket size of internal hash_map object
   *
   * @param n bucket size
   * @return void
   */
  void set_bucket_count(size_t n) {
    vec_.resize(n);
  }

  /**
   * Copy vector
   *
   * @param vec  output vector
   * @return void
   */
  void copy(Vector &vec) const {
    vec.clear();
    for (VecHashMap::const_iterator it = vec_.begin();
         it != vec_.end(); ++it) {
      vec.set(it->first, it->second);
    }
  }

  /**
   * Get value
   *
   * @param key key
   * @return VecValue value
   */
  VecValue get(VecKey key) const {
    VecHashMap::const_iterator it = vec_.find(key);
    if (it != vec_.end()) return it->second;
    else                  return VECTOR_NULL_VALUE;
  }

  /**
   * Set value
   *
   * @param key key
   * @param value value
   * @return void
   */
  void set(VecKey key, VecValue value) {
    vec_[key] = value;
  }

  /**
   * Get size of vector
   *
   * @return size_t size of vector
   */
  size_t size() const {
    return vec_.size();
  }

  /**
   * clear all items in vector
   *
   * @return void
   */
  void clear() {
    vec_.clear();
  }

  /**
   * Get VecHashMap
   *
   * @return VecHashMap pointer of hash_map object
   */
  const VecHashMap *hash_map() const {
    return &vec_;
  }

  /**
   * Get items sorted by value (desc order)
   *
   * @param items sorted keys
   * @return void
   */
  void sorted_items(std::vector<VecItem> &items) const;

  /**
   * Normalize a vector
   *
   * @return void
   */
  void normalize();

  /**
   * Resize a vector
   *
   * @param size resized size
   * @return void
   */
  void resize(size_t size);

  /**
   * Calculate squared norm of a vector
   *
   * @return double squared norm of the vector
   */
  double norm_squared() const;

  /**
   * Calculate norm of a vector
   *
   * @return double norm of the vector
   */
  double norm() const;

  /**
   * Multiply each value of vector by constant
   *
   * @param x constant value
   * @return void
   */
  void multiply_constant(double x);

  /**
   * Add other vector
   * 
   * @param vec input vector
   * @return void
   */
  void add_vector(const Vector &vec);

  /**
   * Delete other vector
   * 
   * @param vec input vector
   * @return void
   */
  void delete_vector(const Vector &vec);

  /**
   * Calculate squared euclid distance between vectors
   *
   * @param vec1 input vector
   * @param vec2 input vector
   * @return double squared distance
   */
  static double euclid_distance_squared(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate euclid distance bewteen bectors
   *
   * @param vec1 input vector
   * @param vec2 input vector
   * @return double distance
   */
  static double euclid_distance(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate inner product value
   *
   * @param vec1 input vector
   * @param vec2 input vector
   * @return double inner product value
   */
  static double inner_product(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate cosine value
   *
   * @param vec1 input vector
   * @param vec2 input vector
   * @return double cosine value
   */
  static double cosine(const Vector &vec1, const Vector &vec2);

  /**
   * Calculate Jaccard coefficient value
   *
   * @param vec1 input vector
   * @param vec2 input vector
   * @return double jaccard coefficient value
   */
  static double jaccard(const Vector &vec1, const Vector &vec2);

  friend std::ostream &operator <<(std::ostream &os, Vector &vec) {
    os.precision(4);
    for (VecHashMap::const_iterator it = vec.vec_.begin();
         it != vec.vec_.end(); ++it) {
      if (it != vec.vec_.begin()) os << " ";
      os << it->first << ":" << it->second;
    }
    return os;
  }
};


/**
 * Compare VecItem for sorting items in a vector
 *
 * @param left   item of vector
 * @param right  item of vector
 * @return bool  return true if left_value > right_value
 */
bool _compare_items(const VecItem &left, const VecItem &right);

} /* namespace bayon */

#endif
