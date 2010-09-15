//
// Document object
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

#ifndef BAYON_DOCUMENT_H_
#define BAYON_DOCUMENT_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "byvector.h"

namespace bayon {

/* typedef */
typedef long DocumentId;  ///< the identifier of a document

/* constants */
const DocumentId DOC_EMPTY_KEY = -1;  ///< empty key for google hash_map


/**
 * Document class.
 */
class Document {
 private:
  DocumentId id_;    /// the identifier of a document
  Vector *feature_;  /// feature vector of a document

 public:
  /**
   * Constructor.
   * @param id the identifier of a document
   */
  explicit Document(DocumentId id) : id_(id) {
    feature_ = new Vector;
  }

  /**
   * Constructor.
   * @param id the identifier of a document
   * @param feature features of a document
   */
  Document(DocumentId id, Vector *feature) : id_(id), feature_(feature) { }

  /**
   * Destructor.
   */
  ~Document() {
    if (feature_) delete feature_;
  }

  /**
   * Get an identifier.
   * @return an identifier
   */
  DocumentId id() const {
    return id_;
  }

  /**
   * Get the pointer of a feature vector
   * @return the pointer of a feature vector
   */
  Vector *feature() {
    return feature_;
  }

  /**
   * Get the const pointer of a eature vector
   * @return the pointer of a feature vector
   */
  const Vector *feature() const {
    return feature_;
  }

  /**
   * Add a feature.
   * @param key the key of a feature
   * @param value the value of a feature
   */
  void add_feature(VecKey key, VecValue value) {
    feature_->set(key, value);
  }

  /**
   * Set features.
   * @param feature a feature vector
   */
  void set_features(Vector *feature) {
    feature_ = feature;
  }

  /**
   * Clear features.
   */
  void clear() {
    feature_->clear();
  }

  /**
   * Apply IDF(inverse document frequency) weighting.
   * @param df document frequencies
   * @param ndocs the number of documents
   */
  void idf(const HashMap<VecKey, size_t>::type &df, size_t ndocs) {
    VecHashMap *hmap = feature()->hash_map();
    HashMap<VecKey, size_t>::type::const_iterator dit;
    for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
      dit = df.find(it->first);
      size_t denom = (dit != df.end()) ? dit->second : 1;
      (*hmap)[it->first] =
        it->second * log(static_cast<double>(ndocs) / denom);
    }
  }
};

}  /* namespace bayon */

#endif  // BAYON_DOCUMENT_H_
