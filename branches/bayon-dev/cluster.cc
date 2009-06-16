//
// Clustering API
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

#include "cluster.h"

namespace bayon {
  
/* Get sorted documents in clusters */
void Cluster::sorted_documents(
  std::vector<std::pair<Document *, double> > &pairs) {
  Vector *centroid = centroid_vector();
  for (size_t i = 0; i < documents_.size(); i++) {
    double similarity = Vector::inner_product(*documents_[i]->feature(),
                                              *centroid);
    pairs.push_back(std::pair<Document *, double>(documents_[i], similarity));
  }
  std::sort(pairs.begin(), pairs.end(), greater_pair<Document *, double>);
}

/* choose documents randomly */
void Cluster::choose_randomly(size_t ndocs, std::vector<Document *> &docs) const {
  HashMap<size_t, bool>::type choosed(ndocs);
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP 
  choosed.set_empty_key(documents_.size());
#endif
  size_t siz = size();
  if (siz < ndocs) ndocs = siz;
  size_t count = 0;
  while (count < ndocs) {
    size_t index = rand() % siz;
    if (choosed.find(index) == choosed.end()) {
      choosed.insert(std::pair<size_t, bool>(index, true));
      docs.push_back(documents_[index]);
      ++count;
    }
  }
}

/* choose documents smartly */
void Cluster::choose_smartly(size_t ndocs, std::vector<Document *> &docs) const {
  HashMap<size_t, double>::type closest(docs.size());
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP 
  closest.set_empty_key(documents_.size());
#endif
  size_t siz = size();
  if (siz < ndocs) ndocs = siz;
  size_t index, count = 0;
  
  index = rand() % siz; // initial center
  docs.push_back(documents_[index]);
  ++count;
  double potential = 0.0;
  for (size_t i = 0; i < documents_.size(); i++) {
    double dist = 1.0 - Vector::inner_product(*documents_[i]->feature(),
                                              *documents_[index]->feature());
    potential += dist;
    closest[i] = dist;
  }

  // choose each center
  while (count < ndocs) {
    double randval = (double)rand() / RAND_MAX * potential;
    for (index = 0; index < documents_.size(); index++) {
      double dist = closest[index];
      if (randval <= dist) break;
      randval -= dist;
    }
    if (index == documents_.size()) index--;
    docs.push_back(documents_[index]);
    ++count;

    double new_potential = 0.0;
    for (size_t i = 0; i < documents_.size(); i++) {
      double dist = 1.0 - Vector::inner_product(*documents_[i]->feature(),
                                                *documents_[index]->feature());
      double min = closest[i];
      if (dist < min) {
        closest[i] = dist;
        min = dist;
      }
      new_potential += min;
    }
    potential = new_potential;
  }
}

/* Set gain when the cluster was sectioned */
void Cluster::set_sectioned_gain() {
  double gain = 0.0;
  if (sectioned_gain_ == 0 && sectioned_clusters_.size() > 1) {
    for (size_t i = 0; i < sectioned_clusters_.size(); i++) {
      gain += sectioned_clusters_[i]->composite_vector()->norm();
    }
    gain -= composite_.norm();
  }
  sectioned_gain_ = gain;
}

/* Section the cluster */
void Cluster::section(size_t nclusters) {
  if (size() < nclusters) return;

  std::vector<Document *> centroids;
  //choose_randomly(nclusters, centroids);
  choose_smartly(nclusters, centroids);
  for (size_t i = 0; i < centroids.size(); i++) {
    Cluster *cluster = new Cluster();
    sectioned_clusters_.push_back(cluster);
  }

  for (size_t i = 0; i < documents_.size(); i++) {
    double max_similarity = -1.0;
    size_t max_index = 0;
    for (size_t j = 0; j < centroids.size(); j++) {
      double similarity = Vector::inner_product(*documents_[i]->feature(),
                                                *centroids[j]->feature());
      if (max_similarity < similarity) {
        max_similarity = similarity;
        max_index = j;
      }
    }
    sectioned_clusters_[max_index]->add_document(documents_[i]);
  }
}

} /* namespace bayon */
