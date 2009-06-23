//
// Cluster Analyzer
// which manages input documents, and executes clustering.
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

#include "analyzer.h"

namespace bayon {

/* Do repeated bisection clustering */
size_t Analyzer::repeated_bisection() {
  Cluster *cluster = new Cluster();
  cluster->set_random_generator(&random_);
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
        sectioned[i]->composite_vector()->clear();
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

  double eval_cluster = 0.0;
  unsigned int loop_count = 0;
  while (loop_count++ < NUM_REFINE_LOOP) {
    std::vector<std::pair<size_t, size_t> > items;
    for (size_t i = 0; i < clusters.size(); i++) {
      for (size_t j = 0; j < clusters[i]->documents().size(); j++) {
        items.push_back(std::pair<size_t, size_t>(i, j));
      }
    }
    random_shuffle(items.begin(), items.end(), random_);

    bool changed = false;
    for (size_t i = 0; i < items.size(); i++) {
      size_t cluster_id = items[i].first;
      size_t item_id    = items[i].second;
      Document *doc = clusters[cluster_id]->documents()[item_id];

      double value_base = refined_vector_value(
        *clusters[cluster_id]->composite_vector(), *doc->feature(), -1);
      double norm_base_moved = sqrt(pow(norms[cluster_id], 2) + value_base);

      double eval_max = -1.0;
      double norm_max = 0.0;
      size_t max_index = 0;
      for (size_t j = 0; j < clusters.size(); j++) {
        if (cluster_id == j) continue;
        double value_target = refined_vector_value(
          *clusters[j]->composite_vector(), *doc->feature(), 1);
        double norm_target_moved = sqrt(pow(norms[j], 2) + value_target);
        double eval_moved = norm_base_moved + norm_target_moved
                            - (norms[cluster_id] + norms[j]);
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

/* Count document frequency of words */
void Analyzer::count_df(HashMap<VecKey, size_t>::type &df) const {
  df.set_empty_key(EMPTY_KEY);
  for (size_t i = 0; i < documents_.size(); i++) {
    VecHashMap *hmap = documents_[i]->feature()->hash_map();
    for (VecHashMap::iterator it = hmap->begin();
         it != hmap->end(); ++it) {
      if (df.find(it->first) == df.end()) df[it->first] = 1;
      else                                df[it->first]++;
    }
  }
}

/* Calc inverse document frequency(IDF) */
void  Analyzer::idf() {
  HashMap<VecKey, size_t>::type df;
  count_df(df);
  size_t ndocs = documents_.size();
  for (size_t i = 0; i < documents_.size(); i++) {
    VecHashMap *hmap = documents_[i]->feature()->hash_map();
    for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
      (*hmap)[it->first] = it->second * log((double)ndocs / (df[it->first] + 1));
    }
  }
}

/* Do k-means clustering */
size_t Analyzer::kmeans() {
  Cluster *cluster = new Cluster;
  cluster->set_random_generator(&random_);
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
size_t Analyzer::do_clustering(const clustering_method &method) {
  size_t num = 0;
  if      (method == KMEANS)             num = kmeans();
  else if (method == REPEATED_BISECTION) num = repeated_bisection();
  return num;
}

} /* namespace bayon */
