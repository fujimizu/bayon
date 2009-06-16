//
// Document 
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

#ifndef BAYON_DOCUMENT_H
#define BAYON_DOCUMENT_H

#include <algorithm>
#include "byvector.h"

namespace bayon {

/* Typedef */
typedef int32_t DocumentId;  // Document ID

/* Constants */
const DocumentId DOCUMENT_ID_EMPTY = -1;

/*********************************************************************
 * Document class
 ********************************************************************/
class Document {
 private:
  /**
   * document id
   */
  DocumentId id_;

  /**
   * features of document
   */
  Vector *feature_;

 public:
  /**
   * Constructor
   *
   * @param id document id
   */
  Document(DocumentId id) : id_(id) {
    feature_ = new Vector;
  }

  /**
   * Constructor
   *
   * @param id document id
   * @param feature features of document
   */
  Document(DocumentId id, Vector *feature) : id_(id), feature_(feature) { }

  /**
   * Destructor
   *
   * if feature_ is null, delete it
   */
  ~Document() {
    if (feature_) delete feature_;
  }

  /**
   * Get document id
   *
   * @return DocumentId  document id
   */
  DocumentId id() const {
    return id_;
  }

  /**
   * Get feature vector
   *
   * @return Vector *  feature vector
   */
  Vector *feature() {
    return feature_;
  }

  /**
   * Get feature vector
   *
   * @return const Vector *  feature vector
   */
  const Vector *feature() const {
    return feature_;
  }

  /**
   * Add feature of document
   *
   * @param key key of feature
   * @param value value of feature
   * @return void
   */
  void add_feature(VecKey key, VecValue value) {
    feature_->set(key, value);
  }

  /**
   * Set feature
   *
   * @param feature Vector object
   * @return void
   */
  void set_feature(Vector *feature) {
    feature_ = feature;
  }

  /**
   * Clear features
   *
   * @return void
   */
  void clear() {
    feature_->clear();
  }
};

} /* namespace bayon */

#endif
