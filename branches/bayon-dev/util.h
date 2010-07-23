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

#ifndef BAYON_UTIL_H
#define BAYON_UTIL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

/* Include hash_map header. */
#include <tr1/unordered_map>

/* Print debug messages. */
#ifdef DEBUG
#define show_log(msg) \
  do { std::cerr << "[log] " << msg << std::endl; } while (false);
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


namespace bayon {

const unsigned int DEFAULT_SEED = 12345;  ///< default seed value
const std::string DELIMITER("\t");        ///< delimiter string


/**
 * Compare pair items.
 *
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
 *
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
 *
 * @param seed seed
 */
void mysrand(unsigned int seed);

/**
 * Get a random number.
 *
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
 * Get random ASCII string.
 * @param max max size of output string
 * @param result output string
 */
void random_string(size_t max, std::string &result);

/**
 * Get file extension.
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

#endif  // BAYON_UTIL_H
