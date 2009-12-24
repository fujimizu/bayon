//
// Clustering API
//
// Copyright(C) 2009  Mizuki Fujisawa <fujisawa@bayon.cc>
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
void Cluster::choose_randomly(size_t ndocs, std::vector<Document *> &docs) {
  HashMap<size_t, bool>::type choosed(ndocs);
  size_t siz = size();
  init_hash_map(siz, choosed);
  if (siz < ndocs) ndocs = siz;
  size_t count = 0;
  while (count < ndocs) {
    size_t index = myrand(&seed_) % siz;
    if (choosed.find(index) == choosed.end()) {
      choosed.insert(std::pair<size_t, bool>(index, true));
      docs.push_back(documents_[index]);
      ++count;
    }
  }
}

/* choose documents smartly */
void Cluster::choose_smartly(size_t ndocs, std::vector<Document *> &docs) {
  HashMap<size_t, double>::type closest(docs.size());
  size_t siz = size();
  init_hash_map(siz, closest);
  if (siz < ndocs) ndocs = siz;
  size_t index, count = 0;

  index = myrand(&seed_) % siz; // initial center
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
    double randval = (double)myrand(&seed_) / RAND_MAX * potential;
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
    cluster->set_seed(seed_);
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

/* Do repeated bisection clustering */
size_t Analyzer::repeated_bisection() {
  Cluster *cluster = new Cluster();
  cluster->set_seed(seed_);
  for (size_t i = 0; i < documents_.size(); i++) {
    cluster->add_document(documents_[i]);
  }

  std::priority_queue<Cluster,
                      std::vector<Cluster *>,
                      CompareClusterBisectionEvalGreater> que;
  cluster->section(2);
  refine_clusters(cluster->sectioned_clusters());
  cluster->set_sectioned_gain();
  cluster->composite_vector()->clear();
  que.push(cluster);

  std::stringstream ss;
  while (!que.empty()) {
    if (limit_nclusters_ > 0 && que.size() >= limit_nclusters_) break;
    cluster = que.top();
    if (cluster->sectioned_clusters().size() < 1) break;
    if (limit_eval_ > 0 && cluster->sectioned_gain() < limit_eval_) break;
    que.pop();
    std::vector<Cluster *> sectioned = cluster->sectioned_clusters();

    // for debug
    ss << "que_size: " << que.size() << "\tcluster_size: " << cluster->size()
       << "\tsectioned: " << sectioned[0]->size() << ", "
       << sectioned[1]->size() << "\tgain: " << cluster->sectioned_gain();
    show_log(ss.str());
    ss.str("");

    for (size_t i = 0; i < sectioned.size(); i++) {
      sectioned[i]->section(2);
      refine_clusters(sectioned[i]->sectioned_clusters());
      sectioned[i]->set_sectioned_gain();
      if (sectioned[i]->sectioned_gain() < limit_eval_) {
        for (size_t j = 0; j < sectioned[i]->sectioned_clusters().size(); j++) {
          sectioned[i]->sectioned_clusters()[j]->clear();
        }
      }
      sectioned[i]->composite_vector()->clear();
      que.push(sectioned[i]);
    }
    delete cluster;
  }
  while (!que.empty()) {
    clusters_.push_back(que.top());
    que.pop();
  }
  std::reverse(clusters_.begin(), clusters_.end());
  return clusters_.size();
}

/* Refine clustering result */
double Analyzer::refine_clusters(std::vector<Cluster *> &clusters) {
  double norms[clusters.size()];
  for (size_t i = 0; i < clusters.size(); i++) {
    norms[i] = clusters[i]->composite_vector()->norm();
  }

  Random r(seed_);
  double eval_cluster = 0.0;
  unsigned int loop_count = 0;
  while (loop_count++ < NUM_REFINE_LOOP) {
    std::vector<std::pair<size_t, size_t> > items;
    for (size_t i = 0; i < clusters.size(); i++) {
      for (size_t j = 0; j < clusters[i]->documents().size(); j++) {
        items.push_back(std::pair<size_t, size_t>(i, j));
      }
    }
    random_shuffle(items.begin(), items.end(), r);

    bool changed = false;
    for (size_t i = 0; i < items.size(); i++) {
      size_t cluster_id = items[i].first;
      size_t item_id    = items[i].second;
      Document *doc = clusters[cluster_id]->documents()[item_id];

      double value_base = refined_vector_value(
        *clusters[cluster_id]->composite_vector(), *doc->feature(), -1);
      double norm_base_moved = pow(norms[cluster_id], 2) + value_base;
      norm_base_moved = norm_base_moved > 0 ? sqrt(norm_base_moved) : 0.0;

      double eval_max = -1.0;
      double norm_max = 0.0;
      size_t max_index = 0;
      for (size_t j = 0; j < clusters.size(); j++) {
        if (cluster_id == j) continue;
        double value_target = refined_vector_value(
          *clusters[j]->composite_vector(), *doc->feature(), 1);
        double norm_target_moved = pow(norms[j], 2) + value_target;
        norm_target_moved = norm_target_moved > 0 ?
          sqrt(norm_target_moved) : 0.0;
        double eval_moved = norm_base_moved + norm_target_moved
                            - norms[cluster_id] - norms[j];
        if (eval_max < eval_moved) {
          eval_max = eval_moved;
          norm_max = norm_target_moved;
          max_index = j;
        }
      }
      if (eval_max > 0) {
        eval_cluster += eval_max;
        clusters[max_index]->add_document(doc);
        clusters[cluster_id]->remove_document(item_id);
        norms[cluster_id] = norm_base_moved;
        norms[max_index] = norm_max;
        changed = true;
      }
    }
    if (!changed) break;
    for (size_t i = 0; i < clusters.size(); i++) {
      clusters[i]->refresh();
    }
  }
  return eval_cluster;
}

double Analyzer::refined_vector_value(const Vector &composite,
                                      const Vector &vec, int sign) {
  double sum = 0.0;
  for (VecHashMap::const_iterator it = vec.hash_map()->begin();
       it != vec.hash_map()->end(); ++it) {
    sum += pow(it->second, 2)
           + sign * 2 * composite.get(it->first) * it->second;
  }
  return sum;
}

/* Count document frequency of vector keys */
void Analyzer::count_df(HashMap<VecKey, size_t>::type &df) const {
  for (size_t i = 0; i < documents_.size(); i++) {
    VecHashMap *hmap = documents_[i]->feature()->hash_map();
    for (VecHashMap::iterator it = hmap->begin();
         it != hmap->end(); ++it) {
      if (df.find(it->first) == df.end()) df[it->first] = 1;
      else                                df[it->first]++;
    }
  }
}

/* Calc inverse document frequency(IDF) and apply it */
void  Analyzer::idf() {
  HashMap<VecKey, size_t>::type df;
  init_hash_map(VECTOR_EMPTY_KEY, df);
  count_df(df);
  size_t ndocs = documents_.size();
  for (size_t i = 0; i < ndocs; i++) {
    documents_[i]->idf(df, ndocs);
  }
}

/* Calc standard score and apply it */
void Analyzer::standard_score() {
  double sum = 0.0;
  double sum_squared = 0.0;
  size_t siz = 0;
  for (size_t i = 0; i < documents_.size(); i++) {
    VecHashMap *hmap = documents_[i]->feature()->hash_map();
    for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
      sum += it->second;
      sum_squared += it->second * it->second;
    }
    siz += hmap->size();
  }
  double ave = sum / siz;
  double variance = sum_squared / siz - ave * ave;
  double sdev = std::sqrt(variance);
  for (size_t i = 0; i < documents_.size(); i++) {
    VecHashMap *hmap = documents_[i]->feature()->hash_map();
    for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
      (*hmap)[it->first] = 10 * (it->second - ave) / sdev + 50;
    }
  }
}

/* Do k-means clustering */
size_t Analyzer::kmeans() {
  Cluster *cluster = new Cluster;
  cluster->set_seed(seed_);
  for (size_t i = 0; i < documents_.size(); i++) {
    cluster->add_document(documents_[i]);
  }
  cluster->section(limit_nclusters_);
  refine_clusters(cluster->sectioned_clusters());
  for (size_t i = 0; i < cluster->sectioned_clusters().size(); i++) {
    cluster->sectioned_clusters()[i]->refresh();
    clusters_.push_back(cluster->sectioned_clusters()[i]);
  }
  delete cluster;
  return clusters_.size();
}

/* Do clustering */
size_t Analyzer::do_clustering(const std::string &mode) {
  size_t num = 0;
  if      (mode == "kmeans") num = kmeans();
  else if (mode == "rb")     num = repeated_bisection();
  return num;
}

void Analyzer::cluster_similarities(Document *document,
  std::vector<std::pair<size_t, double> > &similarities) {
  for (size_t i = 0; i < clusters_.size(); i++) {
    double similarity = Vector::inner_product(*clusters_[i]->centroid_vector(),
                                              *document->feature());
    if (similarity > 0) {
      similarities.push_back(std::pair<size_t, double>(i, similarity));
    }
  }
}

} /* namespace bayon */
