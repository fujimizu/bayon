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

#include <sys/time.h>
#include "util.h"

namespace {
/** characters for random string generation. */
const std::string CHARACTERS(
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz123456789");
} /* namespace */


namespace bayon {

/**
 * Set a seed number for a random number generator.
 */
void mysrand(unsigned int seed) {
#if (_POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
  // do nothing
#else
  srand(seed);
#endif
}

/**
 * Get random number.
 */
int myrand(unsigned int *seed) {
#if (_POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
  return rand_r(seed);
#else
  return rand();
#endif
}

/**
 * Get current time.
 */
double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + static_cast<double>(tv.tv_usec) * 1e-6;
}

/**
 * Get random ASCII string.
 */
void random_string(size_t max, std::string &result) {
  size_t size = static_cast<size_t>(rand() % max);
  if (size == 0) size = 1;
  result.resize(size);
  for (size_t i = 0; i < size; i++) {
    int index = static_cast<int>(rand() % CHARACTERS.size());
    result[i] = CHARACTERS[index];
  }
}

/**
 * Get file extention.
 */
std::string get_extension(const std::string filename) {
  size_t index = filename.rfind('.', filename.size());
  if (index != std::string::npos) {
    return filename.substr(index+1, filename.size()-1);
  }
  return "";
}

/**
 * Split a string by delimiter string.
 */
void split_string(const std::string &s, const std::string &delimiter,
                  std::vector<std::string> &splited) {
  for (size_t i = s.find_first_not_of(delimiter); i != std::string::npos; ) {
    size_t j = s.find_first_of(delimiter, i);
    if (j != std::string::npos) {
      splited.push_back(s.substr(i, j-i));
      i = s.find_first_not_of(delimiter, j+1);
    } else {
      splited.push_back(s.substr(i, s.size()));
      break;
    }
  }
}

} /* namespace bayon */
