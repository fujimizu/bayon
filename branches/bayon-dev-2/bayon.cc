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

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include "bayon.h"

/* typdef */
typedef std::map<std::string, double> Feature;

/* constants */
const std::string DUMMY_OPTARG = "dummy";

/* global variables */
const std::string DELIMITER("\t");
const size_t MAX_VECTOR_ITEM = 50;

/* function prototypes */
int main(int argc, char **argv);
static void usage(std::string progname);
static int parse_options(int argc, char **argv,
                         std::map<std::string, std::string> &option);
static size_t parse_tsv(std::string &tsv, Feature &feature);
static size_t add_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                            std::map<bayon::DocumentId, std::string> &docidmap);
static void show_clusters(bayon::Analyzer &analyzer,
                          std::map<bayon::DocumentId, std::string> &docidmap,
                          bool show_point);
static void show_version();

/* main function */
int main(int argc, char **argv) {
  std::string progname(argv[0]);
  std::map<std::string, std::string> option;
  int optind = parse_options(argc, argv, option);
  if (option.find("version") != option.end()) {
    show_version();
    return 0;
  }
  argc -= optind;
  argv += optind;
  if (argc != 1 || (option.find("number") == option.end()
                    && option.find("limit") == option.end())) {
    usage(progname);
    return 1;
  }

  bayon::Analyzer analyzer;
  if (option.find("seed") != option.end()) {
    unsigned int seed = static_cast<unsigned int>(atoi(option["seed"].c_str()));
    analyzer.set_seed(seed);
  }
  std::map<bayon::DocumentId, std::string> docidmap;

  std::ifstream ifs(argv[0]);
  if (!ifs) {
    std::cerr << "[ERROR]File not found: " << argv[0] << std::endl;
    return 1;
  }
  add_documents(ifs, analyzer, docidmap);
  analyzer.idf();
  analyzer.resize_document_features(MAX_VECTOR_ITEM);
  if (option.find("number") != option.end()) {
    analyzer.set_cluster_size_limit(atoi(option["number"].c_str()));
  } else if (option.find("limit") != option.end()) {
    analyzer.set_eval_limit(atof(option["limit"].c_str()));
  }
  std::string method = "rb"; // default
  if (option.find("method") != option.end()) method = option["method"];
  analyzer.do_clustering(method);

  bool flag_point = (option.find("point") != option.end()) ? true : false;
  show_clusters(analyzer, docidmap, flag_point);

  return 0;
}

/* show usage */
static void usage(std::string progname) {
  std::cerr
    << progname << ": simple and fast clustering tool" << std::endl
    << "Usage:" << std::endl
    << " " << progname << " -n num [-m method] [-p] [-s seed] file" << std::endl
    << " " << progname << " -l limit [-m method] [-p] [-s seed] file" << std::endl
    << "    -n, --number num    ... number of clusters" << std::endl
    << "    -l, --limit lim     ... limit value of cluster bisection" << std::endl
    << "    -m, --method method ... clustering method(rb, kmeans), default:rb" << std::endl
    << "    -p, --point         ... output similairty point" << std::endl
    << "    -s, --seed seed     ... set seed for random number generator" << std::endl
    << "    -v, --version       ... show the version and exit" << std::endl;
}

/* parse command line options */
static int parse_options(int argc, char **argv,
                         std::map<std::string, std::string> &option) {
  int opt;
  extern char *optarg;
  extern int optind;
  while ((opt = getopt(argc, argv, "n:l:m:ps:v")) != -1) {
    switch (opt) {
    case 'n': // number
      option["number"] = optarg;
      break;
    case 'l': // limit
      option["limit"] = optarg;
      break;
    case 'm': // method
      option["method"] = optarg;
      break;
    case 'p': // point
      option["point"] = DUMMY_OPTARG;
      break;
    case 's': // seed
      option["seed"] = optarg;
      break;
    case 'v': // version
      option["version"] = DUMMY_OPTARG;
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

  size_t p = tsv.find(DELIMITER);
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
    tsv = tsv.substr(p + DELIMITER.size());
    p = tsv.find(DELIMITER);
  }
  return keycnt;
}

/* read input dbm and add documents to analyzer */
static size_t add_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                            std::map<bayon::DocumentId, std::string> &docidmap) {
  std::map<std::string, bayon::VecKey> str2num;
  bayon::VecKey item_id = 1;
  bayon::DocumentId doc_id = 1;

  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      size_t p = line.find(DELIMITER);
      std::string doc_name = line.substr(0, p);
      line = line.substr(p + DELIMITER.size());

      bayon::Document doc(doc_id);
      docidmap[doc_id] = doc_name;
      doc_id++;
      Feature feature;
      parse_tsv(line, feature);

      for (Feature::iterator it = feature.begin(); it != feature.end(); ++it) {
        if (str2num.find(it->first) == str2num.end()) {
          str2num[it->first] = item_id++;
        }
        doc.add_feature(str2num[it->first], it->second);
      }
      analyzer.add_document(doc);
    }
  }
  return doc_id;
}

static void show_clusters(bayon::Analyzer &analyzer,
                          std::map<bayon::DocumentId, std::string> &docidmap,
                          bool show_point) {
  bayon::Cluster cluster;
  size_t cluster_count = 1;
  while (analyzer.get_next_result(cluster)) {
    if (cluster.size() > 0) {
      std::vector<std::pair<bayon::Document *, double> > pairs;
      cluster.sorted_documents(pairs);

      std::cout << cluster_count++ << DELIMITER;
      for (size_t i = 0; i < pairs.size(); i++) {
        if (i > 0) std::cout << DELIMITER;
        std::cout << docidmap[pairs[i].first->id()];
        if (show_point) std::cout << DELIMITER << pairs[i].second;
      }
      std::cout << std::endl;
    }
  }
}

/* show version */
static void show_version() {
  std::cout << PACKAGE_STRING << std::endl
            << "Copyright(C) 2009 Mizuki Fujisawa" << std::endl;
}
