//
// Command-line tool
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

#include <getopt.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include "bayon.h"

/********************************************************************
 * Typedef
 *******************************************************************/
typedef enum {
  OPT_NUMBER   = 'n',
  OPT_LIMIT    = 'l',
  OPT_METHOD   = 'm',
  OPT_POINT    = 'p',
  OPT_CLVECTOR = 'c',
  OPT_IDF      = 'i',
  OPT_SEED     = 's',
  OPT_HELP     = 'h',
  OPT_VERSION  = 'v',
} bayon_options;

typedef std::map<bayon_options, std::string> Option;
typedef bayon::HashMap<std::string, double>::type Feature;
typedef bayon::HashMap<bayon::DocumentId, std::string>::type DocIdMap;
typedef bayon::HashMap<bayon::VecKey, std::string>::type VecKeyMap;


/********************************************************************
 * constants
 *******************************************************************/
const std::string DUMMY_OPTARG   = "dummy";
const size_t MAX_VECTOR_ITEM     = 50;
const size_t MAX_SIMILAR_CLUSTER = 20;


/********************************************************************
 * global variables
 *******************************************************************/
struct option longopts[] = {
  {"number",         required_argument, NULL, OPT_NUMBER  },
  {"limit",          required_argument, NULL, OPT_LIMIT   },
  {"method",         required_argument, NULL, OPT_METHOD  },
  {"point",          no_argument,       NULL, OPT_POINT   },
  {"cluster_vector", required_argument, NULL, OPT_CLVECTOR},
  {"idf",            no_argument,       NULL, OPT_IDF     },
  {"seed",           required_argument, NULL, OPT_SEED    },
  {"help",           no_argument,       NULL, OPT_HELP    },
  {"version",        no_argument,       NULL, OPT_VERSION },
  {0, 0, 0, 0}
};


/********************************************************************
 * function prototypes
 *******************************************************************/
int main(int argc, char **argv);
static void usage(std::string progname);
static int parse_options(int argc, char **argv, Option &option);
static size_t parse_tsv(std::string &tsv, Feature &feature);
static size_t add_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                            DocIdMap &docidmap, VecKeyMap &veckeymap);
static void show_clusters(const std::vector<bayon::Cluster *> &clusters,
                          DocIdMap &docidmap, bool show_point);
static void show_multi_clusters(const std::vector<bayon::Cluster *> &clusters,
                                const std::vector<bayon::Document *> &documents,
                                DocIdMap &docidmap, size_t max);
static void save_cluster_vector(std::ofstream &ofs,
                                const std::vector<bayon::Cluster *> &clusters,
                                const VecKeyMap &veckeymap);
static void version();


/* main function */
int main(int argc, char **argv) {
  std::string progname(argv[0]);
  Option option;
  int optind = parse_options(argc, argv, option);
  if (option.find(OPT_VERSION) != option.end()) {
    version();
    return 0;
  }
  argc -= optind;
  argv += optind;
  if (argc != 1 || (option.find(OPT_NUMBER) == option.end()
                    && option.find(OPT_LIMIT) == option.end())) {
    usage(progname);
    return 1;
  }

  bayon::Analyzer analyzer;
  if (option.find(OPT_SEED) != option.end()) {
    unsigned int seed = static_cast<unsigned int>(atoi(option[OPT_SEED].c_str()));
    analyzer.set_seed(seed);
  }

  std::ifstream ifs(argv[0]);
  if (!ifs) {
    std::cerr << "[ERROR]File not found: " << argv[0] << std::endl;
    return 1;
  }
  DocIdMap docidmap;
  bayon::init_hash_map(bayon::DOC_EMPTY_KEY, docidmap);
  VecKeyMap veckeymap;
  bayon::init_hash_map(bayon::VECTOR_EMPTY_KEY, veckeymap);
  add_documents(ifs, analyzer, docidmap, veckeymap);

  if (option.find(OPT_IDF) != option.end()) analyzer.idf();
  analyzer.resize_document_features(MAX_VECTOR_ITEM);

  if (option.find(OPT_NUMBER) != option.end()) {
    analyzer.set_cluster_size_limit(atoi(option[OPT_NUMBER].c_str()));
  } else if (option.find(OPT_LIMIT) != option.end()) {
    analyzer.set_eval_limit(atof(option[OPT_LIMIT].c_str()));
  }
  std::string method = "rb"; // default
  if (option.find(OPT_METHOD) != option.end()) method = option[OPT_METHOD];
  analyzer.do_clustering(method);
  std::vector<bayon::Cluster *> clusters = analyzer.clusters();

  if (option.find(OPT_CLVECTOR) != option.end()) {
    std::ofstream ofs(option[OPT_CLVECTOR].c_str());
    if (!ofs) {
      std::cerr << "[ERROR]Cannot open file: " << option[OPT_CLVECTOR] << std::endl;
      return 1;
    }
    save_cluster_vector(ofs, clusters, veckeymap);
  }
  bool flag_point = (option.find(OPT_POINT) != option.end()) ? true : false;
  if (false) show_multi_clusters(clusters, analyzer.documents(), 
                                 docidmap, MAX_SIMILAR_CLUSTER);
  else      show_clusters(clusters, docidmap, flag_point);

  return 0;
}

/* show usage */
static void usage(std::string progname) {
  std::cerr
    << progname << ": simple and fast clustering tool" << std::endl
    << "Usage:" << std::endl
    << " " << progname << " -n num [-m method] [-p] [-c file] [-i] [-s seed] file" << std::endl
    << " " << progname << " -l limit [-m method] [-p] [-c file] [-i] [-s seed] file" << std::endl
    << "    -n, --number num            number of clusters" << std::endl
    << "    -l, --limit lim             limit value of cluster bisection" << std::endl
    << "    -m, --method method         clustering method(rb, kmeans), default:rb" << std::endl
    << "    -p, --point                 output similarity point" << std::endl
    << "    -c, --cluster-vector file   save vectors of cluster centroids" << std::endl
    << "    -i, --idf                   apply idf to input vectors" << std::endl
    << "    -s, --seed seed             set seed for random number generator" << std::endl
    << "    -v, --version               show the version and exit" << std::endl;
}

/* parse command line options */
static int parse_options(int argc, char **argv, Option &option) {
  int opt;
  extern char *optarg;
  extern int optind;
  while ((opt = getopt_long(argc, argv, "n:l:m:pc:is:hv", longopts, NULL))
         != -1) {
    switch (opt) {
    case OPT_NUMBER:
      option[OPT_NUMBER] = optarg;
      break;
    case OPT_LIMIT:
      option[OPT_LIMIT] = optarg;
      break;
    case OPT_METHOD:
      option[OPT_METHOD] = optarg;
      break;
    case OPT_POINT:
      option[OPT_POINT] = DUMMY_OPTARG;
      break;
    case OPT_CLVECTOR:
      option[OPT_CLVECTOR] = optarg;
      break;
    case OPT_IDF:
      option[OPT_POINT] = DUMMY_OPTARG;
      break;
    case OPT_SEED:
      option[OPT_SEED] = optarg;
      break;
    case OPT_HELP:
      option[OPT_HELP] = DUMMY_OPTARG;
      break;
    case OPT_VERSION:
      option[OPT_VERSION] = DUMMY_OPTARG;
      break;
    default:
      break;
    }
  }
  return optind;
}

/* parse tsv format string */
static size_t parse_tsv(std::string &tsv, Feature &feature) {
  std::string key;
  int cnt = 0;
  size_t keycnt = 0;

  size_t p = tsv.find(bayon::DELIMITER);
  while (true) {
    std::string s = tsv.substr(0, p);
    if (cnt % 2 == 0) {
      key = s;
    } else {
      double point = 0.0;
      point = atof(s.c_str());
      if (!key.empty() && point != 0) {
        feature[key] = point;
        keycnt++;
      }
    }
    if (p == tsv.npos) break;
    cnt++;
    tsv = tsv.substr(p + bayon::DELIMITER.size());
    p = tsv.find(bayon::DELIMITER);
  }
  return keycnt;
}

/* read input dbm and add documents to analyzer */
static size_t add_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                            DocIdMap &docidmap, VecKeyMap &veckeymap) {
  std::map<std::string, bayon::VecKey> str2num;
  bayon::VecKey item_id = 1;
  bayon::DocumentId doc_id = 1;

  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      size_t p = line.find(bayon::DELIMITER);
      std::string doc_name = line.substr(0, p);
      line = line.substr(p + bayon::DELIMITER.size());

      bayon::Document doc(doc_id);
      docidmap[doc_id] = doc_name;
      doc_id++;
      Feature feature;
      bayon::init_hash_map("", feature);
      parse_tsv(line, feature);

      for (Feature::iterator it = feature.begin(); it != feature.end(); ++it) {
        if (str2num.find(it->first) == str2num.end()) {
          str2num[it->first] = item_id;
          veckeymap[item_id] = it->first;
          item_id++;
        }
        doc.add_feature(str2num[it->first], it->second);
      }
      analyzer.add_document(doc);
    }
  }
  return doc_id;
}

/* show clustering result */
static void show_clusters(const std::vector<bayon::Cluster *> &clusters,
                          DocIdMap &docidmap, bool show_point) {
  size_t cluster_count = 1;
  for (size_t i = 0; i < clusters.size(); i++) {
    if (clusters[i]->size() > 0) {
      std::vector<std::pair<bayon::Document *, double> > pairs;
      clusters[i]->sorted_documents(pairs);

      std::cout << cluster_count++ << bayon::DELIMITER;
      for (size_t i = 0; i < pairs.size(); i++) {
        if (i > 0) std::cout << bayon::DELIMITER;
        std::cout << docidmap[pairs[i].first->id()];
        if (show_point) std::cout << bayon::DELIMITER << pairs[i].second;
      }
      std::cout << std::endl;
    }
  }
}

/* ... */
static void show_multi_clusters(const std::vector<bayon::Cluster *> &clusters,
                                const std::vector<bayon::Document *> &documents,
                                DocIdMap &docidmap, size_t max) {
  bayon::Classifier classifier;
  size_t cluster_count = 1;
  for (size_t i = 0; i < clusters.size(); i++) {
    classifier.add_vector(cluster_count++, *clusters[i]->centroid_vector());
  }

  for (size_t i = 0; i < documents.size(); i++) {
    std::vector<std::pair<bayon::VectorId, double> > pairs;
    classifier.similar_vectors(*documents[i]->feature(), pairs);

    std::cout << docidmap[documents[i]->id()];
    for (size_t j = 0; j < pairs.size() && j < max; j++) {
      std::cout << bayon::DELIMITER << pairs[j].first
                << bayon::DELIMITER << pairs[j].second;
    }
    std::cout << std::endl;
  }
}

/* save vectors of cluster centroids */
static void save_cluster_vector(std::ofstream &ofs,
                                const std::vector<bayon::Cluster *> &clusters,
                                const VecKeyMap &veckeymap) {
  size_t cluster_count = 1;
  for (size_t i = 0; i < clusters.size(); i++) {
    if (clusters[i]->size() > 0) {
      bayon::Vector *vec = clusters[i]->centroid_vector();
      ofs << cluster_count++;
      for (bayon::VecHashMap::const_iterator it = vec->hash_map()->begin();
           it != vec->hash_map()->end(); ++it) {
        ofs << bayon::DELIMITER;
        VecKeyMap::const_iterator itv = veckeymap.find(it->first);
        if (itv != veckeymap.end()) ofs << itv->second;
        else                        ofs << it->first;
        ofs << bayon::DELIMITER << it->second;
      }
      ofs << std::endl;
    }
  }
}

/* show version */
static void version() {
  std::cout << PACKAGE_STRING << std::endl
            << "Copyright(C) 2009 " << AUTHOR << std::endl;
}
