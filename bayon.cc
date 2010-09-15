//
// Command-line tool
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

#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "bayon.h"

/********************************************************************
 * Typedef
 *******************************************************************/
typedef enum {
  OPT_NUMBER   = 'n',
  OPT_LIMIT    = 'l',
  OPT_POINT    = 'p',
  OPT_CLVECTOR = 'c',
  OPT_CLVECTOR_SIZE,
  OPT_METHOD,
  OPT_SEED,
  OPT_CLASSIFY = 'C',
  OPT_INV_KEYS,
  OPT_INV_SIZE,
  OPT_CLASSIFY_SIZE,
  OPT_VECTOR_SIZE,
  OPT_IDF,
  OPT_HELP     = 'h',
  OPT_VERSION  = 'v',
} bayon_options;

typedef std::map<bayon_options, std::string> Option;
typedef bayon::HashMap<std::string, double>::type Feature;
typedef bayon::HashMap<bayon::DocumentId, std::string>::type DocId2Str;
typedef bayon::HashMap<bayon::VecKey, std::string>::type VecKey2Str;
typedef bayon::HashMap<std::string, bayon::VecKey>::type Str2VecKey;


/********************************************************************
 * constants
 *******************************************************************/
const std::string DUMMY_OPTARG("dummy");
const size_t DEFAULT_MAX_CLVECTOR    = 50;
const size_t DEFAULT_MAX_CLASSIFY    = 20;
const size_t DEFAULT_MAX_INDEX_KEY   = 20;
const size_t DEFAULT_MAX_INDEX       = 100;
const bayon::VecKey VEC_START_KEY    = 0;
const bayon::DocumentId DOC_START_ID = 0;


/********************************************************************
 * global variables
 *******************************************************************/
struct option longopts[] = {
  {"number",        required_argument, NULL, OPT_NUMBER       },
  {"limit",         required_argument, NULL, OPT_LIMIT        },
  {"point",         no_argument,       NULL, OPT_POINT        },
  {"clvector",      required_argument, NULL, OPT_CLVECTOR     },
  {"clvector-size", required_argument, NULL, OPT_CLVECTOR_SIZE},
  {"method",        required_argument, NULL, OPT_METHOD       },
  {"seed",          required_argument, NULL, OPT_SEED         },
  {"classify",      required_argument, NULL, OPT_CLASSIFY     },
  {"inv-keys",      required_argument, NULL, OPT_INV_KEYS     },
  {"inv-size",      required_argument, NULL, OPT_INV_SIZE     },
  {"classify-size", required_argument, NULL, OPT_CLASSIFY_SIZE},
  {"vector-size",   required_argument, NULL, OPT_VECTOR_SIZE  },
  {"idf",           no_argument,       NULL, OPT_IDF          },
  {"help",          no_argument,       NULL, OPT_HELP         },
  {"version",       no_argument,       NULL, OPT_VERSION      },
  {0, 0, 0, 0}
};


/********************************************************************
 * function prototypes
 *******************************************************************/
int main(int argc, char **argv);
static void usage(std::string progname);
static int parse_options(int argc, char **argv, Option &option);
static size_t parse_tsv(std::string &tsv, Feature &feature);
static void read_document(std::string &str, bayon::Document &doc,
                          bayon::VecKey &veckey, DocId2Str &docid2str,
                          VecKey2Str &veckey2str, Str2VecKey &str2veckey);
static size_t read_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                             bayon::VecKey &veckey, DocId2Str &docid2str,
                             VecKey2Str &veckey2str, Str2VecKey &str2veckey);
static size_t read_classifier_vectors(size_t max_index,
                                      std::ifstream &ifs,
                                      bayon::Classifier &classifier,
                                      bayon::VecKey &veckey,
                                      DocId2Str &claid2str,
                                      VecKey2Str &veckey2str,
                                      Str2VecKey &str2veckey);
static void show_clusters(const std::vector<bayon::Cluster *> &clusters,
                          DocId2Str &docid2str, bool show_point);
static void show_classified(size_t max_keys, size_t max_output,
                            const bayon::Classifier &classifer,
                            const bayon::Document &document,
                            const DocId2Str &docid2str,
                            const DocId2Str &claid2str);
static void save_cluster_vector(size_t max_vec, std::ofstream &ofs,
                                const std::vector<bayon::Cluster *> &clusters,
                                const VecKey2Str &veckey2str);
static int execute_clustering(const Option &option, std::ifstream &ifs_doc);
static int execute_classification(const Option &option, std::ifstream &ifs_doc);
static void version();


/* main function */
int main(int argc, char **argv) {
  std::string progname(argv[0]);
  Option option;
  int optind = parse_options(argc, argv, option);
  if (option.find(OPT_VERSION) != option.end()) {
    version();
    return EXIT_SUCCESS;
  }
  if (option.find(OPT_HELP) != option.end()) {
    usage(progname);
    return EXIT_SUCCESS;
  }
  argc -= optind;
  argv += optind;
  if (argc != 1 || (option.find(OPT_NUMBER) == option.end()
                    && option.find(OPT_LIMIT) == option.end()
                    && option.find(OPT_CLASSIFY) == option.end())) {
    usage(progname);
    return EXIT_FAILURE;
  }
  std::ifstream ifs_doc(argv[0]);
  if (!ifs_doc) {
    fprintf(stderr, "[ERROR]File not found: %s\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (option.find(OPT_CLASSIFY) != option.end()) {
    /* classification */
    return execute_classification(option, ifs_doc);
  } else {
    /* clustering */
    return execute_clustering(option, ifs_doc);
  }
}

/* show usage */
static void usage(std::string progname) {
  fprintf(stderr, "%s: simple and fast clustering tool\n\n", progname.c_str());
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "* Clustering input data\n");
  fprintf(stderr, " %% %s -n num [options] file\n", progname.c_str());
  fprintf(stderr, " %% %s -l limit [options] file\n", progname.c_str());
  fprintf(stderr, "    -n, --number=num      the number of clusters\n");
  fprintf(stderr, "    -l, --limit=lim       limit value of cluster bisection\n");
  fprintf(stderr, "    -p, --point           output similarity points\n");
  fprintf(stderr, "    -c, --clvector=file   save vectors of cluster centroids\n");
  fprintf(stderr, "    --clvector-size=num   max size of output vectors of\n");
  fprintf(stderr, "                          cluster centroids (default: %ld)\n",
          DEFAULT_MAX_CLVECTOR);
  fprintf(stderr, "    --method=method       clustering method(rb, kmeans), default:rb\n");
  fprintf(stderr, "    --seed=seed           set a seed for random number generator\n\n");
  fprintf(stderr, "* Get the similar clusters for each input documents\n");
  fprintf(stderr, " %% %s -C file [options] file\n", progname.c_str());
  fprintf(stderr, "    -C, --classify=file   target vectors\n");
  fprintf(stderr, "    --inv-keys=num        max size of the keys of each vector to be\n");
  fprintf(stderr, "                          looked up in inverted index (default: %ld)\n",
          DEFAULT_MAX_INDEX_KEY);
  fprintf(stderr, "    --inv-size=num        max size of the inverted index of each key\n");
  fprintf(stderr, "                          (default: %ld)\n", DEFAULT_MAX_INDEX);
  fprintf(stderr, "    --classify-size=num   max size of output similar groups\n");
  fprintf(stderr, "                          (default: %ld)\n\n", DEFAULT_MAX_CLASSIFY);
  fprintf(stderr, "* Common options\n");
  fprintf(stderr, "    --vector-size=num     max size of each input vector\n");
  fprintf(stderr, "    --idf                 apply idf to input vectors\n");
  fprintf(stderr, "    -h, --help            show help messages\n");
  fprintf(stderr, "    -v, --version         show the version and exit\n");
}

/* parse command line options */
static int parse_options(int argc, char **argv, Option &option) {
  int opt;
  extern char *optarg;
  extern int optind;
  while ((opt = getopt_long(argc, argv, "n:l:pc:C:hv", longopts, NULL))
         != -1) {
    switch (opt) {
    case OPT_NUMBER:
      option[OPT_NUMBER] = optarg;
      break;
    case OPT_LIMIT:
      option[OPT_LIMIT] = optarg;
      break;
    case OPT_POINT:
      option[OPT_POINT] = DUMMY_OPTARG;
      break;
    case OPT_CLVECTOR:
      option[OPT_CLVECTOR] = optarg;
      break;
    case OPT_CLVECTOR_SIZE:
      option[OPT_CLVECTOR_SIZE] = optarg;
      break;
    case OPT_METHOD:
      option[OPT_METHOD] = optarg;
      break;
    case OPT_SEED:
      option[OPT_SEED] = optarg;
      break;
    case OPT_CLASSIFY:
      option[OPT_CLASSIFY] = optarg;
      break;
    case OPT_INV_KEYS:
      option[OPT_INV_KEYS] = optarg;
      break;
    case OPT_INV_SIZE:
      option[OPT_INV_SIZE] = optarg;
      break;
    case OPT_CLASSIFY_SIZE:
      option[OPT_CLASSIFY_SIZE] = optarg;
      break;
    case OPT_VECTOR_SIZE:
      option[OPT_VECTOR_SIZE] = optarg;
      break;
    case OPT_IDF:
      option[OPT_IDF] = DUMMY_OPTARG;
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

/* parse input string and make a Document object */
static void read_document(std::string &str, bayon::Document &doc,
                          bayon::VecKey &veckey, DocId2Str &docid2str,
                          VecKey2Str &veckey2str, Str2VecKey &str2veckey) {
  size_t p = str.find(bayon::DELIMITER);
  std::string doc_name = str.substr(0, p);
  str = str.substr(p + bayon::DELIMITER.size());
  docid2str[doc.id()] = doc_name;

  Feature feature;
  bayon::init_hash_map("", feature);
  parse_tsv(str, feature);
  for (Feature::iterator it = feature.begin(); it != feature.end(); ++it) {
    if (str2veckey.find(it->first) == str2veckey.end()) {
      str2veckey[it->first] = veckey;
      veckey2str[veckey] = it->first;
      veckey++;
    }
    doc.add_feature(str2veckey[it->first], it->second);
  }
}

/* read input file and add documents to analyzer */
static size_t read_documents(std::ifstream &ifs, bayon::Analyzer &analyzer,
                             bayon::VecKey &veckey, DocId2Str &docid2str,
                             VecKey2Str &veckey2str, Str2VecKey &str2veckey) {
  bayon::DocumentId docid = DOC_START_ID;
  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      bayon::Document doc(docid);
      read_document(line, doc, veckey, docid2str, veckey2str, str2veckey);
      analyzer.add_document(doc);
      docid++;
    }
  }
  return docid;
}

/* read input file and add vectors to classifier */
static size_t read_classifier_vectors(size_t max_index,
                                      std::ifstream &ifs,
                                      bayon::Classifier &classifier,
                                      bayon::VecKey &veckey,
                                      DocId2Str &claid2str,
                                      VecKey2Str &veckey2str,
                                      Str2VecKey &str2veckey) {
  bayon::DocumentId claid = 0;
  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      size_t p = line.find(bayon::DELIMITER);
      std::string name = line.substr(0, p);
      line = line.substr(p + bayon::DELIMITER.size());
      claid2str[claid] = name;
      Feature feature;
      bayon::init_hash_map("", feature);
      parse_tsv(line, feature);
      bayon::Vector vec;
      for (Feature::iterator it = feature.begin(); it != feature.end(); ++it) {
        if (str2veckey.find(it->first) == str2veckey.end()) {
          str2veckey[it->first] = veckey;
          veckey2str[veckey] = it->first;
          veckey++;
        }
        vec.set(str2veckey[it->first], it->second);
      }
      classifier.add_vector(claid, vec);
      claid++;
    }
  }
  classifier.resize_inverted_index(max_index);
  return 0;
}

/* show clustering result */
static void show_clusters(const std::vector<bayon::Cluster *> &clusters,
                          DocId2Str &docid2str, bool show_point) {
  size_t cluster_count = 1;
  for (size_t i = 0; i < clusters.size(); i++) {
    if (clusters[i]->size() > 0) {
      std::vector<std::pair<bayon::Document *, double> > pairs;
      clusters[i]->sorted_documents(pairs);

      printf("%ld%s", cluster_count++, bayon::DELIMITER.c_str());
      for (size_t i = 0; i < pairs.size(); i++) {
        if (i > 0) printf("%s", bayon::DELIMITER.c_str());
        printf("%s", docid2str[pairs[i].first->id()].c_str());
        if (show_point) printf("%s%f", bayon::DELIMITER.c_str(), pairs[i].second);
      }
      printf("\n");
    }
  }
}

/* show classified result */
static void show_classified(size_t max_keys, size_t max_output,
                            const bayon::Classifier &classifier,
                            const bayon::Document &document,
                            const DocId2Str &docid2str,
                            const DocId2Str &claid2str) {
  std::vector<std::pair<bayon::Classifier::VectorId, double> > pairs;
  classifier.similar_vectors(max_keys, *document.feature(), pairs);

  DocId2Str::const_iterator it = docid2str.find(document.id());
  if (it != docid2str.end()) {
    printf("%s", it->second.c_str());
  } else {
    printf("%ld", document.id());
  }
  for (size_t j = 0; j < pairs.size() && j < max_output; j++) {
    DocId2Str::const_iterator it = claid2str.find(pairs[j].first);
    printf("%s", bayon::DELIMITER.c_str());
    if (it != claid2str.end()) {
      printf("%s", it->second.c_str());
    } else {
      printf("%ld", pairs[j].first);
    }
    printf("%s%f", bayon::DELIMITER.c_str(), pairs[j].second);
  }
  printf("\n");
}

/* save vectors of cluster centroids */
static void save_cluster_vector(size_t max_vec, std::ofstream &ofs,
                                const std::vector<bayon::Cluster *> &clusters,
                                const VecKey2Str &veckey2str) {
  size_t cluster_count = 1;
  for (size_t i = 0; i < clusters.size(); i++) {
    if (clusters[i]->size() > 0) {
      std::vector<bayon::VecItem> items;
      clusters[i]->centroid_vector()->sorted_items_abs(items);
      ofs << cluster_count++;
      for (size_t i = 0; i < items.size() && i < max_vec; i++) {
        ofs << bayon::DELIMITER;
        VecKey2Str::const_iterator itv = veckey2str.find(items[i].first);
        if (itv != veckey2str.end()) ofs << itv->second;
        else                         ofs << items[i].first;
        ofs << bayon::DELIMITER << items[i].second;
      }
      ofs << std::endl;
    }
  }
}

static int execute_clustering(const Option &option, std::ifstream &ifs_doc) {
  DocId2Str docid2str;
  bayon::init_hash_map(bayon::DOC_EMPTY_KEY, docid2str);
  VecKey2Str veckey2str;
  bayon::init_hash_map(bayon::VECTOR_EMPTY_KEY, veckey2str);
  Str2VecKey str2veckey;
  bayon::init_hash_map("", str2veckey);
  bayon::VecKey veckey = VEC_START_KEY;

  bayon::Analyzer analyzer;
  read_documents(ifs_doc, analyzer, veckey, docid2str, veckey2str, str2veckey);
  Option::const_iterator oit;
  if (option.find(OPT_IDF) != option.end()) analyzer.idf();
  if ((oit = option.find(OPT_VECTOR_SIZE)) != option.end())
    analyzer.resize_document_features(atoi(oit->second.c_str()));
  if ((oit = option.find(OPT_SEED)) != option.end()) {
    unsigned int seed = static_cast<unsigned int>(atoi(oit->second.c_str()));
    analyzer.set_seed(seed);
  }
  if ((oit = option.find(OPT_NUMBER)) != option.end()) {
    int nclusters = atoi(oit->second.c_str());
    if (nclusters < 1) {
      fprintf(stderr, "[ERROR]The number of clusters must be more than zero: ");
      fprintf(stderr, "\"%s\"\n", oit->second.c_str());
      return EXIT_FAILURE;
    }
    analyzer.set_cluster_size_limit(nclusters);
  } else if ((oit = option.find(OPT_LIMIT)) != option.end()) {
    analyzer.set_eval_limit(atof(oit->second.c_str()));
  }
  bayon::Analyzer::Method method = bayon::Analyzer::RB;
  if ((oit = option.find(OPT_METHOD)) != option.end()) {
    if (oit->second == "kmeans") {
      method = bayon::Analyzer::KMEANS;
    } else if (oit->second == "rb") {
      // do nothing
    } else {
      fprintf(stderr, "[ERROR]Illegal clustering method: %s\n",
              oit->second.c_str());
      return EXIT_FAILURE;
    }
  }
  analyzer.do_clustering(method);
  std::vector<bayon::Cluster *> clusters = analyzer.clusters();

  bool flag_point = (option.find(OPT_POINT) != option.end()) ? true : false;
  show_clusters(clusters, docid2str, flag_point);

  if ((oit = option.find(OPT_CLVECTOR)) != option.end()) {
    std::ofstream ofs(oit->second.c_str());
    if (!ofs) {
      fprintf(stderr, "[ERROR]Cannot open file: %s\n", oit->second.c_str());
      return EXIT_FAILURE;
    }
    size_t max_vec = ((oit = option.find(OPT_CLVECTOR_SIZE)) != option.end()) ?
      atoi(oit->second.c_str()) : DEFAULT_MAX_CLVECTOR;
    save_cluster_vector(max_vec, ofs, clusters, veckey2str);
  }
  return EXIT_SUCCESS;
}

static int execute_classification(const Option &option,
                                  std::ifstream &ifs_doc) {
  DocId2Str docid2str;
  bayon::init_hash_map(bayon::DOC_EMPTY_KEY, docid2str);
  VecKey2Str veckey2str;
  bayon::init_hash_map(bayon::VECTOR_EMPTY_KEY, veckey2str);
  Str2VecKey str2veckey;
  bayon::init_hash_map("", str2veckey);
  bayon::VecKey veckey = VEC_START_KEY;
  bayon::DocumentId docid;
  std::string line;
  size_t ndocs = 0;
  bayon::HashMap<bayon::VecKey, size_t>::type df;
  bayon::init_hash_map(bayon::VECTOR_EMPTY_KEY, df);

  if (option.find(OPT_IDF) != option.end()) {
    docid = DOC_START_ID;
    while (std::getline(ifs_doc, line)) {
      bayon::Document doc(docid);
      read_document(line, doc, veckey, docid2str, veckey2str, str2veckey);
      bayon::VecHashMap *hmap = doc.feature()->hash_map();
      for (bayon::VecHashMap::iterator it = hmap->begin();
        it != hmap->end(); ++it) {
        if (df.find(it->first) == df.end()) df[it->first] = 1;
        else                                df[it->first]++;
      }
      ndocs++;
    }
    ifs_doc.clear();
    ifs_doc.seekg(0, std::ios_base::beg);
  }

  bayon::Classifier classifier;
  Option::const_iterator oit = option.find(OPT_CLASSIFY);
  std::ifstream ifs_cla(oit->second.c_str());
  if (!ifs_cla) {
    fprintf(stderr, "[ERROR]File not found: %s\n", oit->second.c_str());
    return EXIT_FAILURE;
  }
  size_t max_keys = (oit = option.find(OPT_INV_KEYS)) != option.end() ?
    atoi(oit->second.c_str()) : DEFAULT_MAX_INDEX_KEY;
  size_t max_index = (oit = option.find(OPT_INV_SIZE)) != option.end() ?
    atoi(oit->second.c_str()) : DEFAULT_MAX_INDEX;
  size_t max_output = (oit = option.find(OPT_CLASSIFY_SIZE)) != option.end() ?
    atoi(oit->second.c_str()) : DEFAULT_MAX_CLASSIFY;

  DocId2Str claid2str;
  bayon::init_hash_map(bayon::DOC_EMPTY_KEY, claid2str);
  read_classifier_vectors(max_index, ifs_cla, classifier, veckey,
                          claid2str, veckey2str, str2veckey);
  docid = DOC_START_ID;
  while (std::getline(ifs_doc, line)) {
    bayon::Document doc(docid);
    read_document(line, doc, veckey, docid2str, veckey2str, str2veckey);
    if (option.find(OPT_IDF) != option.end()) doc.idf(df, ndocs);
    if ((oit = option.find(OPT_VECTOR_SIZE)) != option.end())
      doc.feature()->resize(atoi(oit->second.c_str()));
    doc.feature()->normalize();
    show_classified(max_keys, max_output, classifier,
                    doc, docid2str, claid2str);
  }
  return EXIT_SUCCESS;
}


/* show version */
static void version() {
#ifdef PACKAGE_NAME
  printf("%s", PACKAGE_NAME);
#else
  printf("bayon");
#endif
#ifdef PACKAGE_VERSION
  printf(" version %s", PACKAGE_VERSION);
#endif
  printf("\n");
  printf("Copyright(C) 2010");
#ifdef AUTHOR
  printf("  %s", AUTHOR);
#endif
  printf("\n");
}
