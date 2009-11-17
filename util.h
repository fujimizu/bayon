//
// Utility functions
//
// Copyright(C) 2009  Mizuki Fujisawa <fujisawa@bayon.cc>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdlib>
#include <iostream>

/* include hash_map header */
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
#include <google/dense_hash_map>
#elif HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#else
#include <map>
#endif

/* Print debug messages */
#ifdef DEBUG
#define show_log(msg) \
  do { std::cerr << "[log] " << msg << std::endl; } while (false);
#else
#define show_log(msg) \
  do { } while (false);
#endif

/* isnan for win32 */
#ifdef _WIN32
#include <cfloat>
#define isnan(x) _isnan(x)
#endif


/* hash function of string key for __gnu_cxx::hash_map */
#if !defined(HAVE_GOOGLE_DENSE_HASH_MAP) && defined(HAVE_EXT_HASH_MAP)
namespace __gnu_cxx {
  template<> struct hash<std::string> {
    size_t operator() (const std::string &x) const {
      return hash<const char *>()(x.c_str());
    }
  };
}
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


/********************************************************************
 * Constatns
 *******************************************************************/
/* default seed value for random number generator */
const unsigned int DEFAULT_SEED = 12345;

/* delimiter string */
const std::string DELIMITER("\t");


/********************************************************************
 * Functions
 *******************************************************************/
/**
 * Initialize hash_map object (for google::dense_hash_map)
 *
 * @param empty_key key of empty entry
 * @param deleted_key key of deleted entry
 * @return void
 */
template<typename KeyType, typename HashType>
void init_hash_map(const KeyType &empty_key, HashType &hmap) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  hmap.max_load_factor(0.9);
  hmap.set_empty_key(empty_key);
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

/**
 * Compare pair items by absolute value
 *
 * @param left  item
 * @param right item
 * @return bool return true if abs(left_value) > abs(right_value)
 */
template<typename KeyType, typename ValueType>
bool greater_pair_abs(const std::pair<KeyType, ValueType> &left,
                      const std::pair<KeyType, ValueType> &right) {
  return std::abs(left.second) > std::abs(right.second);
}

/**
 * Set seed for random number generator
 *
 * @param seed seed
 * @return void
 */
void mysrand(unsigned int seed);

/**
 * Get random number
 *
 * @param seed pointer of seed
 * @return int random number
 */
int myrand(unsigned int *seed);


/********************************************************************
 * Classes
 *******************************************************************/
/**
 * Random number generator class
 */
class Random {
 private:
  unsigned int seed_;

 public:
  Random() : seed_(DEFAULT_SEED) { }
  Random(unsigned int seed) : seed_(seed) { }
  ~Random() { }

  void set_seed(unsigned int seed) { seed_ = seed; }
  unsigned int operator()(unsigned int max) {
    double ratio = static_cast<double>(myrand(&seed_))
                   / static_cast<double>(RAND_MAX);
    return static_cast<unsigned int>(ratio * max);
  }
};

} /* namespace bayon */

#endif
