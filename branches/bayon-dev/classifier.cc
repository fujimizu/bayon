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

#include "classifier.h"

namespace bayon {

void Classifier::similar_clusters(
  const Document &document, 
  std::vector<std::pair<size_t, double> > &items) const {
  for (HashMap<size_t, Vector>::type::const_iterator it = vectors_.begin();
       it != vectors_.end(); ++it) {
    double similarity = Vector::inner_product(it->second,
                                              *document.feature());
    if (similarity != 0) {
      items.push_back(std::pair<size_t, double>(it->first, similarity));
    }
  }
  std::sort(items.begin(), items.end(), greater_pair<size_t, double>);
}

} // namespace bayon
