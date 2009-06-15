//
// Document Classifier
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

#ifndef BAYON_CLASSIFIER_H
#define BAYON_CLASSIFIER_H

#include <utility>
#include <vector>
#include "byvector.h"
#include "cluster.h"

namespace bayon {

/*********************************************************************
 * Classifier class
 ********************************************************************/
class Classifier {
 private:
  HashMap<size_t, Vector>::type vectors_;

 public:
  void similar_clusters(
    const Document &document,
    std::vector<std::pair<size_t, double> > &items) const;
};

} // namespace bayon

#endif
