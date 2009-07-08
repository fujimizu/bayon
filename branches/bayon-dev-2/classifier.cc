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

#include "classifier.h"

namespace bayon {

/* Add vector keys to inverted index */
void Classifier::add_inverted_index(VectorId id, const Vector &vec) {
  std::vector<VecItem> items;
  vec.sorted_items(items);
  for (size_t i = 0; i < items.size() && i < MAX_VECTOR_KEY; i++) {
    if (inverted_index_.find(items[i].first) == inverted_index_.end()) {
      std::vector<VectorId> *v = new std::vector<VectorId>;
      v->push_back(id);
      inverted_index_[items[i].first] = v;
    } else {
      inverted_index_[items[i].first]->push_back(id);
    }
  }
}

/* Look up inverted index */
size_t Classifier::lookup_inverted_index(
  const Vector &vec, std::vector<VectorId> &ids) const {
  HashMap<VectorId, bool>::type idmap;
  init_hash_map(VECID_EMPTY_KEY, idmap);
  for (VecHashMap::const_iterator it = vec.hash_map()->begin();
       it != vec.hash_map()->end(); ++it) {
    InvertedIndex::const_iterator itidx = inverted_index_.find(it->first);
    if (itidx != inverted_index_.end()) {
      for (size_t i = 0; i < itidx->second->size(); i++) {
        idmap[itidx->second->at(i)] = true;
      }
    }
  }

  for (HashMap<VectorId, bool>::type::iterator it = idmap.begin();
       it != idmap.end(); ++it) ids.push_back(it->first);
  return ids.size();
}

/* Get list of id and points of similar vectors */
void Classifier::similar_vectors(
  const Vector &vec, 
  std::vector<std::pair<VectorId, double> > &items,
  bool use_inverted_index) const {

  if (use_inverted_index) { // inverted index
    std::vector<VectorId> ids;
    lookup_inverted_index(vec, ids);
    for (size_t i = 0; i < ids.size(); i++) {
      HashMap<VectorId, Vector>::type::const_iterator it = vectors_.find(ids[i]);
      if (it != vectors_.end()) {
        double similarity = Vector::inner_product(it->second, vec);
        if (similarity != 0) {
          items.push_back(std::pair<VectorId, double>(it->first, similarity));
        }
      }
    }
  } else { // all
    for (HashMap<VectorId, Vector>::type::const_iterator it = vectors_.begin();
         it != vectors_.end(); ++it) {
      double similarity = Vector::inner_product(it->second, vec);
      if (similarity != 0) {
        items.push_back(std::pair<VectorId, double>(it->first, similarity));
      }
    }
  }

  std::sort(items.begin(), items.end(), greater_pair<VectorId, double>);
}

} /* namespace bayon */
