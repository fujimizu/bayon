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
#include "cluster.h"
#include <unistd.h> 

typedef std::map<std::string, double> Feature;

/* global variables */
const std::string DELIMITER = "\t";
const size_t MAX_VECTOR_ITEM = 50;
std::string g_progname;  // program name

/* function prototypes */
int main(int argc, char **argv);
static void usage();
static size_t parse_tsv(std::string &tsv, Feature &feature, size_t max);
static size_t add_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                            std::map<int, std::string> &docidmap);
static void parse_options(int argc, char **argv,
                          std::map<std::string, std::string> &option);

/* main function */
int main(int argc, char **argv) {
  g_progname = argv[0];
  std::map<std::string, std::string> option;
  parse_options(argc, argv, option);
  if (option.find("input") == option.end()
      || (option.find("number") == option.end()
          && option.find("limit") == option.end())) {
    usage();
    return 1;
  }

  srand((unsigned int)time(NULL));
  bayon::Analyzer analyzer;
  std::map<int, std::string> docidmap;

  std::ifstream ifs(option["input"].c_str());
  if (!ifs) {
    std::cerr << "[ERROR]File not found: " << option["input"] << std::endl;
    return 1;
  }
  add_documents(ifs, analyzer, docidmap);
  if (option.find("number") != option.end()) {
    analyzer.set_cluster_size_limit(atoi(option["number"].c_str()));
  } else if (option.find("limit") != option.end()) {
    analyzer.set_eval_limit(atof(option["limit"].c_str()));
  }
  std::string method = "rb"; // default
  if (option.find("method") != option.end()) method = option["method"];
  analyzer.do_clustering(method);

  bayon::Cluster cluster;
  size_t cluster_count = 1;
  while (analyzer.get_next_result(cluster)) {
    if (cluster.size() > 0) {
      std::cout << cluster_count++ << "\t";
      for (size_t i = 0; i < cluster.documents().size(); i++) {
        if (i > 0) std::cout << " ";
        std::cout << docidmap[cluster.documents()[i]->id()];
      }
      std::cout << std::endl;
    }
  }
  
  return 0;
}

/* show usage */
static void usage() {
  std::cerr
    << g_progname << ": Clustering Tool" << std::endl
    << "Usage:" << std::endl
    << " " << g_progname << " -n num -i file [-m method]" << std::endl
    << " " << g_progname << " -l limit -i file [-m method]" << std::endl
    << "    -n, --number num    ... number of clusters" << std::endl
    << "    -l, --limit lim     ... limit value of cluster bisection" << std::endl
    << "    -i, --input path    ... input file" << std::endl
    << "    -m, --method method ... clustering method(rb, kmeans), default:rb" << std::endl;
}

/* parse tsv format string */
static size_t parse_tsv(std::string &tsv, Feature &feature, size_t max) {
  std::string word;
  int cnt = 0;
  size_t kwcnt = 0;

  size_t p = tsv.find(DELIMITER);
  while (max > 0 && kwcnt < max) {
    std::string s = tsv.substr(0, p);
    if (cnt % 2 == 0) {
      word = s;
    } else {
      double point = 0.0;
      point = atof(s.c_str());
      if (!word.empty() && point > 0) {
        feature[word] = point;
        kwcnt++;
      }
    }
    if (p == tsv.npos) break;
    cnt++;
    tsv = tsv.substr(p + DELIMITER.size());
    p = tsv.find(DELIMITER);
  }
  return kwcnt;
}

/* read input dbm and add documents to analyzer */
static size_t add_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                            std::map<int, std::string> &docidmap) {
  std::map<std::string, int> str2int;
  int item_id = 1;
  int doc_id = 1;

  std::string line;
  while (std::getline(ifs, line)) {
    size_t p = line.find(DELIMITER);
    std::string doc_name = line.substr(0, p);
    line = line.substr(p + DELIMITER.size());

    bayon::Document doc(doc_id);
    docidmap[doc_id] = doc_name;
    doc_id++;
    Feature feature;
    parse_tsv(line, feature, MAX_VECTOR_ITEM);

    for (Feature::iterator it = feature.begin(); it != feature.end(); ++it) {
      if (str2int.find(it->first) == str2int.end()) {
        str2int[it->first] = item_id++;
      }
      doc.add_feature(str2int[it->first], it->second);
    }
    analyzer.add_document(doc);
  }
  return doc_id;
}

/* parse command line options */
static void parse_options(int argc, char **argv,
                          std::map<std::string, std::string> &option) {
  int opt;
  extern char *optarg;
  while ((opt = getopt(argc, argv, "n:i:l:m:")) != -1) {
    switch (opt) {
    case 'n': // number
      option["number"] = optarg;
      break;
    case 'i': // input
      option["input"] = optarg;
      break;
    case 'l': // limit
      option["limit"] = optarg;
      break;
    case 'm': // method
      option["method"] = optarg;
      break;
    default:
      break;
    }
  }
}
