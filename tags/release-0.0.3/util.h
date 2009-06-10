//
// Utilities
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
 * Print debug messages
 */
#ifdef DEBUG
#define show_log(msg) \
  do { std::cerr << "[log] " << msg << std::endl; } while (false);
#else
#define show_log(msg) \
  do { } while (false);
#endif

#endif
