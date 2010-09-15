//
// Utility functions
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

#ifndef BAYON_UTIL_H_
#define BAYON_UTIL_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

/* include hash_map headers. */
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
#include <google/dense_hash_map>
#elif HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#else
#include <map>
#endif

/* Print debug messages. */
#ifdef DEBUG
#define show_log(msg) \
  do { fprintf(stderr, "[log] %s\n", msg); } while (false);
#else
#define show_log(msg) \
  do { } while (false);
#endif

/* isnan */
#ifndef isnan
#ifdef _WIN32
#include <cfloat>
#define isnan(x) _isnan(x)
#else
#define isnan(x) (x != x)
#endif
#endif

/* hash function of string key for __gnu_cxx::hash_map */
#if (defined(_WIN32) || !defined(HAVE_GOOGLE_DENSE_HASH_MAP)) && defined(HAVE_EXT_HASH_MAP)
namespace __gnu_cxx {
  template<> struct hash<std::string> {
    size_t operator() (const std::string &x) const {
      return hash<const char *>()(x.c_str());
    }
  };
  template<> struct hash<long long> {
    size_t operator()(long long __x) const {
      return __x;
    }
  };
  template<> struct hash<unsigned long long> {
    size_t operator()(unsigned long  long __x) const {
      return __x;
    }
  };
}
#endif


namespace bayon {

/**
 * typedef template of hash map class
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

const unsigned int DEFAULT_SEED = 12345;  ///< default seed value
const std::string DELIMITER("\t");        ///< delimiter string
const double MAX_LOAD_FACTOR = 0.9;       ///< max load factor of hash_map


/**
 * Initialize a hash_map object (for google::dense_hash_map)
 * @param empty_key key of empty entry
 * @param deleted_key key of deleted entry
 */
template<typename KeyType, typename HashType>
void init_hash_map(const KeyType &empty_key, HashType &hmap) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  hmap.max_load_factor(MAX_LOAD_FACTOR);
  hmap.set_empty_key(empty_key);
#endif
}

/**
 * Initialize a hash_map object (for google::dense_hash_map)
 * @param empty_key key of a empty entry
 * @param deleted_key key of a deleted entry
 * @param bucket_count bucket count
 */
template<typename KeyType, typename HashType>
void init_hash_map(const KeyType &empty_key, HashType &hmap,
                   size_t bucket_count) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  hmap.rehash(bucket_count);
  hmap.max_load_factor(MAX_LOAD_FACTOR);
  hmap.set_empty_key(empty_key);
#endif
}

/**
 * Compare pair items.
 * @param left item
 * @param right item
 * @return return true if left_value > right_value
 */
template<typename KeyType, typename ValueType>
bool greater_pair(const std::pair<KeyType, ValueType> &left,
                  const std::pair<KeyType, ValueType> &right) {
  if (left.second > right.second) {
    return true;
  } else if (left.second == right.second) {
    return left.first > right.first;
  } else {
    return false;
  }
}

/**
 * Compare pair items by absolute values.
 * @param left item
 * @param right item
 * @return return true if abs(left_value) > abs(right_value)
 */
template<typename KeyType, typename ValueType>
bool greater_pair_abs(const std::pair<KeyType, ValueType> &left,
                      const std::pair<KeyType, ValueType> &right) {
  ValueType absleft = std::abs(left.second);
  ValueType absright = std::abs(right.second);
  if (absleft > absright) {
    return true;
  } else if (absleft == absright) {
    return left.first > right.first;
  } else {
    return false;
  }
}

/**
 * Set a seed for a random number generator.
 * @param seed seed
 */
void mysrand(unsigned int seed);

/**
 * Get a random number.
 * @param seed the pointer of a seed
 * @return a random number
 */
int myrand(unsigned int *seed);

/**
 * Get current time.
 * @return current time
 */
double get_time();

/**
 * Get a random ASCII string.
 * @param max max size of output string
 * @param result output string
 */
void random_string(size_t max, std::string &result);

/**
 * Get a file extension.
 * @param filename file name
 * @return extension
 */
std::string get_extension(const std::string filename);

/**
 * Split a string by delimiter string
 * @param s input string to be splited
 * @param delimiter delimiter string
 * @param splited splited strings
 */
void split_string(const std::string &s, const std::string &delimiter,
                  std::vector<std::string> &splited);


/**
 * Random number generator class.
 */
class Random {
 private:
  unsigned int seed_;   ///< a seed number

 public:
  /**
   * Constructor.
   */
  Random() : seed_(DEFAULT_SEED) { }

  /**
   * Constructor.
   * @param seed a seed number
   */
  explicit Random(unsigned int seed) : seed_(seed) { }

  /**
   * Destructor
   */
  ~Random() { }

  /**
   * Set a seed number.
   * @param seed a seed number
   */
  void set_seed(unsigned int seed) { seed_ = seed; }

  /**
   * Operator()
   * @param max maximum number
   * return a random number
   */
  unsigned int operator()(unsigned int max) {
    double ratio = static_cast<double>(myrand(&seed_))
                   / static_cast<double>(RAND_MAX);
    return static_cast<unsigned int>(ratio * max);
  }
};

} /* namespace bayon */

#endif  // BAYON_UTIL_H_
