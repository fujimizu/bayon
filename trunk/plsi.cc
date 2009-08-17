//
// pLSI (probabilistic latent semantic indexing)
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
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include "bayon.h"


/********************************************************************
 * Typedef
 *******************************************************************/
typedef enum {
  OPT_NUMBER = 'n',
  OPT_ITER   = 'i',
  OPT_BETA   = 'b',
} plsi_options;

typedef std::map<plsi_options, std::string> Option;
typedef bayon::HashMap<std::string, double>::type Feature;
typedef bayon::HashMap<bayon::DocumentId, std::string>::type DocId2Str;
typedef bayon::HashMap<bayon::VecKey, std::string>::type VecKey2Str;
typedef bayon::HashMap<std::string, bayon::VecKey>::type Str2VecKey;


/********************************************************************
 * constants
 *******************************************************************/
const size_t DEFAULT_NUM_ITER   = 50;
const double DEFAULT_BETA       = 0.75;
const unsigned int DEFAULT_SEED = 12345;


/********************************************************************
 * global variables
 *******************************************************************/
struct option longopts[] = {
  {"number", required_argument, NULL, OPT_NUMBER},
  {"iter",   required_argument, NULL, OPT_ITER  },
  {"beta",   required_argument, NULL, OPT_BETA  },
  {0, 0, 0, 0}
};


/********************************************************************
 * class
 *******************************************************************/
namespace bayon {

class PLSI {
 private:
  size_t num_cluster_;
  size_t num_doc_;
  size_t num_word_;
  double beta_;
  unsigned int seed_;
  std::vector<Document *> documents_;
  double **pdz_, **pdz_new_;
  double **pwz_, **pwz_new_;
  double *pz_,   *pz_new_;
  double ***scores_;

  void set_random_probabilities(size_t row, size_t col, double **array) {
    for (size_t i = 0; i < row; i++) {
      array[i] = new double[col];
      double sum = 0.0;
      for (size_t j = 0; j < col; j++) {
        array[i][j] = myrand(&seed_);
        sum += array[i][j];
      }
      for (size_t j = 0; j < col; j++) {
        array[i][j] /= sum;
      }
    }
  }

  void expect() {
    for (size_t id = 0; id < num_doc_; id++) {
      VecHashMap *hmap = documents_[id]->feature()->hash_map();
      for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
        double denominator = 0.0;
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          denominator += pz_[iz]
                         * pow(pwz_[it->first][iz] * pdz_[id][iz], beta_);
        }
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          double numerator = pz_[iz]
                             * pow(pwz_[it->first][iz] * pdz_[id][iz], beta_);
          scores_[id][it->first][iz] = denominator ?
            numerator / denominator : 0.0;
        }
      }
    }
  }

  void maximize() {
    double denominators[num_cluster_];
    double denom_sum = 0.0;

    for (size_t iz = 0; iz < num_cluster_; iz++) {
      denominators[iz] = 0.0;
      for (size_t id = 0; id < num_doc_; id++) {
        VecHashMap *hmap = documents_[id]->feature()->hash_map();
        for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
          double val = it->second * scores_[id][it->first][iz];
          denominators[iz] += val;
          pdz_new_[id][iz] += val;
          pwz_new_[it->first][iz] += val;
        }
      }
      denom_sum += denominators[iz];

      for (size_t id = 0; id < num_doc_; id++) {
        pdz_[id][iz] = pdz_new_[id][iz] / denominators[iz];
        pdz_new_[id][iz] = 0.0;
      }
      for (size_t iw = 0; iw < num_word_; iw++) {
        pwz_[iw][iz] = pwz_new_[iw][iz] / denominators[iz];
        pwz_new_[iw][iz] = 0.0;
      }
    }
    for (size_t iz = 0; iz < num_cluster_; iz++)
      pz_[iz] = denominators[iz] / denom_sum;
  }

 public:
  PLSI(size_t num_cluster, double beta, unsigned int seed)
    : num_cluster_(num_cluster), num_doc_(0), num_word_(0),
      beta_(beta), seed_(seed) { }

  ~PLSI() {
    for (size_t i = 0; i < num_doc_; i ++) {
      delete pdz_[i];
      delete pdz_new_[i];
    }
    for (size_t i = 0; i < num_word_; i ++) {
      delete pwz_[i];
      delete pwz_new_[i];
    }
    delete [] pdz_;
    delete [] pwz_;
    delete [] pz_;
    delete [] pdz_new_;
    delete [] pwz_new_;
    delete [] pz_new_;
  }

  void init_probabilities() {
    pdz_ = new double*[num_doc_];
    set_random_probabilities(num_doc_, num_cluster_, pdz_);
    pwz_ = new double*[num_word_];
    set_random_probabilities(num_word_, num_cluster_, pwz_);
    pz_ = new double[num_cluster_];
    for (size_t i = 0; i < num_cluster_; i++) pz_[i] = 1.0 / num_cluster_;

    pdz_new_ = new double*[num_doc_];
    for (size_t id = 0; id < num_doc_; id++) {
      pdz_new_[id] = new double[num_cluster_];
      for (size_t iz = 0; iz < num_cluster_; iz++) pdz_new_[id][iz] = 0.0;
    }
    pwz_new_ = new double*[num_word_];
    for (size_t iw = 0; iw < num_word_; iw++) {
      pwz_new_[iw] = new double[num_cluster_];
      for (size_t iz = 0; iz < num_cluster_; iz++) pwz_new_[iw][iz] = 0.0;
    }
    pz_new_ = new double[num_cluster_];
    for (size_t iz = 0; iz < num_cluster_; iz++) pz_new_[iz] = 0.0;

    scores_ = new double**[num_doc_];
    for (size_t i = 0; i < num_doc_; i++) {
      scores_[i] = new double*[num_word_];
      for (size_t j = 0; j < num_word_; j++) {
        scores_[i][j] = new double[num_cluster_];
      }
    }
  }

  void add_document(Document &doc) {
    Document *ptr = new Document(doc.id(), doc.feature());
    doc.set_features(NULL);
    documents_.push_back(ptr);
    num_doc_++;
    VecHashMap *hmap = ptr->feature()->hash_map();
    for (VecHashMap::iterator it = hmap->begin();
         it != hmap->end(); ++it) {
      if ((it->first+1) > static_cast<int>(num_word_)) num_word_ = it->first+1;
    }
  }

  void em(size_t num_iter) {
    for (size_t i = 0; i < num_iter; i++) {
      expect();
      maximize();
    }
  }

  void show_pdz() {
    for (size_t i = 0; i < num_doc_; i++) {
      for (size_t j = 0; j < num_cluster_; j++) {
        if (j != 0) std::cout << "\t";
        std::cout << pdz_[i][j];
      }
      std::cout << std::endl;
    }
  }

  void show_pwz() {
    for (size_t i = 0; i < num_word_; i++) {
      for (size_t j = 0; j < num_cluster_; j++) {
        if (j != 0) std::cout << "\t";
        std::cout << pwz_[i][j];
      }
      std::cout << std::endl;
    }
  }

  void show_pz() {
    for (size_t i = 0; i < num_cluster_; i++) {
      if (i != 0) std::cout << "\t";
      std::cout << pz_[i];
    }
    std::cout << std::endl;
  }

  double **pdz() { return pdz_; }
  double **pwz() { return pwz_; }
  double *pz()   { return pz_; }
  const std::vector<Document *> &documents() { return documents_; }
};

} // namespace bayon


/********************************************************************
 * function prototypes
 *******************************************************************/
int main(int argc, char **argv);
static void usage(std::string progname);
static int parse_options(int argc, char **argv, Option &option);
static size_t parse_tsv(std::string &tsv, Feature &feature);
static size_t read_documents(std::ifstream &ifs, bayon::PLSI &plsi,
                      bayon::VecKey &veckey, DocId2Str &docid2str,
                      VecKey2Str &veckey2str, Str2VecKey &str2veckey);

/* main function */
int main(int argc, char **argv) {
  std::string progname(argv[0]);
  Option option;
  int optind = parse_options(argc, argv, option);
  argc -= optind;
  argv += optind;
  if (argc != 1 || option.find(OPT_NUMBER) == option.end()) {
    usage(progname);
    return EXIT_FAILURE;
  }

  std::ifstream ifs_doc(argv[0]);
  if (!ifs_doc) {
    std::cerr << "[ERROR]File not found: " << argv[0] << std::endl;
    return EXIT_FAILURE;
  }

  DocId2Str docid2str;
  bayon::init_hash_map(bayon::DOC_EMPTY_KEY, docid2str);
  VecKey2Str veckey2str;
  bayon::init_hash_map(bayon::VECTOR_EMPTY_KEY, veckey2str);
  Str2VecKey str2veckey;
  bayon::init_hash_map("", str2veckey);
  bayon::VecKey veckey = 0;

  size_t num_cluster = static_cast<size_t>(atoi(option[OPT_NUMBER].c_str()));
  size_t num_iter = option.find(OPT_ITER) != option.end() ?
    static_cast<size_t>(atoi(option[OPT_ITER].c_str())) : DEFAULT_NUM_ITER;
  double beta = option.find(OPT_ITER) != option.end() ?
    atof(option[OPT_BETA].c_str()) : DEFAULT_BETA;
  bayon::PLSI plsi(num_cluster, beta, DEFAULT_SEED);;
  size_t num_doc = read_documents(ifs_doc, plsi, veckey,
                                  docid2str, veckey2str, str2veckey);
  plsi.init_probabilities();
  plsi.em(num_iter);

  double **pdz = plsi.pdz();
  for (size_t i = 0; i < num_doc; i++) {
    std::cout << docid2str[plsi.documents()[i]->id()];
    for (size_t j = 0; j < num_cluster; j++) {
      std::cout << "\t" << pdz[i][j];
    }
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}

/* show usage */
static void usage(std::string progname) {
  std::cerr
    << progname << ": Clustering tool by probabilistic latent semantic indexing"
    << std::endl
    << "Usage:" << std::endl
    << " % " << progname << " -n num [-b beta | -i niter] file" << std::endl
    << "    -n, --number=num      number of clusters" << std::endl
    << "    -i, --iter=num        number of iteration" << std::endl
    << "    -b, --beta=double     parameter of tempered EM" << std::endl;
}

/* parse command line options */
static int parse_options(int argc, char **argv, Option &option) {
  int opt;
  extern char *optarg;
  extern int optind;
  while ((opt = getopt_long(argc, argv, "n:i:b:", longopts, NULL))
         != -1) {
    switch (opt) {
    case OPT_NUMBER:
      option[OPT_NUMBER] = optarg;
      break;
    case OPT_ITER:
      option[OPT_ITER] = optarg;
      break;
    case OPT_BETA:
      option[OPT_BETA] = optarg;
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

/* read input file and add documents to PLSI object */
static size_t read_documents(std::ifstream &ifs, bayon::PLSI &plsi,
                             bayon::VecKey &veckey, DocId2Str &docid2str,
                             VecKey2Str &veckey2str, Str2VecKey &str2veckey) {
  bayon::DocumentId docid = 0;
  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      size_t p = line.find(bayon::DELIMITER);
      std::string doc_name = line.substr(0, p);
      line = line.substr(p + bayon::DELIMITER.size());

      bayon::Document doc(docid);
      docid2str[docid] = doc_name;
      docid++;
      Feature feature;
      bayon::init_hash_map("", feature);
      parse_tsv(line, feature);

      for (Feature::iterator it = feature.begin(); it != feature.end(); ++it) {
        if (str2veckey.find(it->first) == str2veckey.end()) {
          str2veckey[it->first] = veckey;
          veckey2str[veckey] = it->first;
          veckey++;
        }
        doc.add_feature(str2veckey[it->first], it->second);
      }
      plsi.add_document(doc);
    }
  }
  return docid;
}

