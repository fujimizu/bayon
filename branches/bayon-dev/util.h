//
// Utility functions
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

#ifndef BAYON_UTIL_H
#define BAYON_UTIL_H

#include <iostream>
#include "config.h"

// include hash_map header
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
#include <google/dense_hash_map>
#elif HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#else
#include <map>
#endif

namespace bayon {

/********************************************************************
 * Typedef
 *******************************************************************/
/**
 * typedef HashMap
 *
 * 'google::dense_hash_map' or '__gnu__cxx::hash_map' or 'std::map'
 */
template<typename KeyType, typename ValueType>
struct HashMap {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  typedef google::dense_hash_map<KeyType, ValueType> type;
#elif HAVE_EXT_HASH_MAP
  typedef __gnu_cxx::hash_map<KeyType, ValueType> type;
#else
  typedef std::map<KeyType, ValueType> type;
#endif
};

/**
 * typedef long int
 */
#if SIZEOF_LONG == 8
typedef int64_t LongType;   // key of vector
#else
typedef int32_t LongType;
#endif

/********************************************************************
 * Constatns
 *******************************************************************/
/* empty key for google::dense_hash_map */
const LongType EMPTY_KEY        = -1;

/* deleted key for google::dense_hash_map */
const LongType DELETED_KEY      = -2;

/********************************************************************
 * Classes
 *******************************************************************/
/**
 * Random number generator class
 */
class Random {
 private:
  /* seed */
  unsigned int seed_;

  /* default seed value for random number generator */
  static const unsigned int DEFAULT_SEED = 1;

 public:
  /**
   * Constructor
   */
  Random() : seed_(DEFAULT_SEED) { }

  /**
   * Constructor
   *
   * @param seed seed of rand
   */
  Random(unsigned int seed) : seed_(seed) {
#if (_POSIX_C_SOURCE < 1) && !defined(_XOPEN_SOURCE) && !defined(_POSIX_SOURCE)
    srand(seed_);
#endif
  }

  /**
   * Set seed
   *
   * @param seed seed
   * @return void
   */
  void set_seed(unsigned int seed) {
    seed_ = seed;
#if (_POSIX_C_SOURCE < 1) && !defined(_XOPEN_SOURCE) && !defined(_POSIX_SOURCE)
    srand(seed_);
#endif
  }

  /**
   * Set random number [0, RAND_MAX]
   *
   * @return int random number
   */
  int randint() {
#if (_POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
    return rand_r(&seed_);
#else
    return rand();
#endif
  }

  /**
   * get random number [0, max)
   *
   * @param max max random number
   * @return unsigned int random number
   */
  unsigned int operator()(unsigned int max) {
    double ratio =  static_cast<double>(randint()) / static_cast<double>(RAND_MAX);
    return static_cast<unsigned int>(ratio * max);
  }
};

/********************************************************************
 * Macro and Functions
 *******************************************************************/
/**
 * Print debug messages
 *
 * @param msg debug message
 */
#ifdef DEBUG
#define show_log(msg) \
  do { std::cerr << "[log] " << msg << std::endl; } while (false);
#else
#define show_log(msg) \
  do { } while (false);
#endif

/**
 * Initialize hash_map object (for google::dense_hash_map)
 *
 * @param empty_key key of empty entry
 * @param deleted_key key of deleted entry
 * @return void
 */
template<typename KeyType, typename HashType>
void init_hash_map(const KeyType &empty_key, const KeyType &deleted_key,
                   HashType &hmap) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  hmap.max_load_factor(0.9);
  hmap.set_empty_key(empty_key);
  hmap.set_deleted_key(deleted_key);
#endif
}

/**
 * Compare pair items
 *
 * @param left  item
 * @param right item
 * @return bool return true if left_value > right_value
 */
template<typename KeyType, typename ValueType>
bool greater_pair(const std::pair<KeyType, ValueType> &left,
                  const std::pair<KeyType, ValueType> &right) {
  return left.second > right.second;
}

} /* namespace bayon */

#endif
