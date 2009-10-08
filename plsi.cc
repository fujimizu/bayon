//
// pLSI (probabilistic latent semantic indexing)
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
// I refer the following pLSI package written by Taku Kudo.
// (http://chasen.org/~taku/software/plsi/)
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
  OPT_NORMALIZE,
} plsi_options;

typedef std::map<plsi_options, std::string> Option;
typedef bayon::HashMap<std::string, double>::type Feature;
typedef bayon::HashMap<bayon::DocumentId, std::string>::type DocId2Str;
typedef bayon::HashMap<bayon::VecKey, std::string>::type VecKey2Str;
typedef bayon::HashMap<std::string, bayon::VecKey>::type Str2VecKey;


/********************************************************************
 * constants
 *******************************************************************/
const std::string DUMMY_OPTARG  = "dummy";
const size_t DEFAULT_NUM_ITER   = 50;
const double DEFAULT_BETA       = 0.75;
const unsigned int DEFAULT_SEED = 12345;


/********************************************************************
 * global variables
 *******************************************************************/
struct option longopts[] = {
  {"number",    required_argument, NULL, OPT_NUMBER   },
  {"iter",      required_argument, NULL, OPT_ITER     },
  {"beta",      required_argument, NULL, OPT_BETA     },
  {"normalize", no_argument,       NULL, OPT_NORMALIZE},
  {0, 0, 0, 0}
};


/********************************************************************
 * class
 *******************************************************************/
namespace bayon {

class PLSI {
 private:
  std::vector<Document *> documents_;
  size_t num_cluster_;
  size_t num_doc_;
  size_t num_word_;
  double beta_;
  double sum_weight_;
  unsigned int seed_;
  double **pdz_, **pdz_new_;
  double **pwz_, **pwz_new_;
  double *pz_,   *pz_new_;

  void set_random_prob(size_t row, size_t col,
                       double **array, double **array_new) {
    for (size_t i = 0; i < row; i++) {
      array[i] = new double[col];
      array_new[i] = new double[col];
      double sum = 0.0;
      for (size_t j = 0; j < col; j++) {
        array[i][j] = myrand(&seed_);
        array_new[i][j] = 0.0;
        sum += array[i][j];
      }
      for (size_t j = 0; j < col; j++) {
        array[i][j] /= sum;
      }
    }
  }

  void em_loop() {
    for (size_t id = 0; id < num_doc_; id++) {
      VecHashMap *hmap = documents_[id]->feature()->hash_map();
      for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
        double denom = 0.0;
        double numers[num_cluster_];
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          numers[iz] = pz_[iz] * pow(pwz_[it->first][iz] * pdz_[id][iz], beta_);
          denom += numers[iz];
        }
        if (denom == 0.0) continue;
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          double score = it->second * numers[iz] / denom;
          pdz_new_[id][iz]        += score;
          pwz_new_[it->first][iz] += score;
          pz_new_[iz]             += score;
        }
      }
    }

    for (size_t iz = 0; iz < num_cluster_; iz++) {
      for (size_t id = 0; id < num_doc_; id++) {
        pdz_[id][iz] = pdz_new_[id][iz] / pz_new_[iz];
        pdz_new_[id][iz] = 0.0;
      }
      for (size_t iw = 0; iw < num_word_; iw++) {
        pwz_[iw][iz] = pwz_new_[iw][iz] / pz_new_[iz];
        pwz_new_[iw][iz] = 0.0;
      }
      pz_[iz] = pz_new_[iz] / sum_weight_;
      pz_new_[iz] = 0.0;
    }
  }

 public:
  PLSI(size_t num_cluster, double beta, unsigned int seed)
    : num_cluster_(num_cluster), num_doc_(0), num_word_(0),
      beta_(beta), sum_weight_(0.0), seed_(seed),
      pdz_(NULL), pdz_new_(NULL), pwz_(NULL), pwz_new_(NULL),
      pz_(NULL), pz_new_(NULL) { }

  ~PLSI() {
    for (size_t i = 0; i < documents_.size(); i++) delete documents_[i];

    for (size_t i = 0; i < num_doc_; i ++) {
      if (pdz_ && pdz_[i])         delete [] pdz_[i];
      if (pdz_new_ && pdz_new_[i]) delete [] pdz_new_[i];
    }
    for (size_t i = 0; i < num_word_; i ++) {
      if (pwz_ && pwz_[i])         delete [] pwz_[i];
      if (pwz_new_ && pwz_new_[i]) delete [] pwz_new_[i];
    }
    if (pdz_)     delete [] pdz_;
    if (pdz_new_) delete [] pdz_new_;
    if (pwz_)     delete [] pwz_;
    if (pwz_new_) delete [] pwz_new_;
    if (pz_)      delete [] pz_;
    if (pz_new_)  delete [] pz_new_;
  }

  void init_prob() {
    pdz_     = new double*[num_doc_];
    pdz_new_ = new double*[num_doc_];
    set_random_prob(num_doc_, num_cluster_, pdz_, pdz_new_);

    pwz_     = new double*[num_word_];
    pwz_new_ = new double*[num_word_];
    set_random_prob(num_word_, num_cluster_, pwz_, pwz_new_);

    pz_     = new double[num_cluster_];
    pz_new_ = new double[num_cluster_];
    for (size_t iz = 0; iz < num_cluster_; iz++) {
      pz_[iz] = 1.0 / num_cluster_;
      pz_new_[iz] = 0;
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
      sum_weight_ += it->second;
    }
  }

  void em(size_t num_iter) {
    for (size_t i = 0; i < num_iter; i++) em_loop();
  }

  void show_pdz() const {
    //std::cout.setf(std::ios::fixed, std::ios::floatfield);
    for (size_t i = 0; i < num_doc_; i++) {
      for (size_t j = 0; j < num_cluster_; j++) {
        if (j != 0) std::cout << "\t";
        std::cout << pdz_[i][j];
      }
      std::cout << std::endl;
    }
  }

  void show_pwz() const {
    //std::cout.setf(std::ios::fixed, std::ios::floatfield);
    for (size_t i = 0; i < num_word_; i++) {
      for (size_t j = 0; j < num_cluster_; j++) {
        if (j != 0) std::cout << "\t";
        std::cout << pwz_[i][j];
      }
      std::cout << std::endl;
    }
  }

  void show_pz() const {
    //std::cout.setf(std::ios::fixed, std::ios::floatfield);
    for (size_t i = 0; i < num_cluster_; i++) {
      if (i != 0) std::cout << "\t";
      std::cout << pz_[i];
    }
    std::cout << std::endl;
  }

  void show_membership(
    const HashMap<bayon::DocumentId, std::string>::type &docid2str,
    bool normalize = false) const {
    HashMap<bayon::DocumentId, std::string>::type::const_iterator it;
    //std::cout.setf(std::ios::fixed, std::ios::floatfield);
    for (size_t id = 0; id < num_doc_; id++) {
      it = docid2str.find(documents_[id]->id());
      if (it != docid2str.end()) std::cout << it->second;
      else                       std::cout << documents_[id]->id();

      if (normalize) {
        double sum = 0.0;
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          sum += pdz_[id][iz] * pz_[iz];
        }
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          double val = sum == 0.0 ? 0 : pdz_[id][iz] * pz_[iz] / sum;
          std::cout << "\t" << val;
        }
      } else {
        for (size_t iz = 0; iz < num_cluster_; iz++) {
          std::cout << "\t" << pdz_[id][iz] * pz_[iz];
        }
      }
      std::cout << std::endl;
    }
  }
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

  size_t num_cluster = static_cast<size_t>(atoi(option[OPT_NUMBER].c_str()));
  if (num_cluster <= 0) {
    std::cerr << "[ERROR] The number of output cluster must be greater than zero."
              << std::endl;
    return EXIT_FAILURE;
  }
  size_t num_iter = option.find(OPT_ITER) != option.end() ?
    static_cast<size_t>(atoi(option[OPT_ITER].c_str())) : DEFAULT_NUM_ITER;
  if (num_cluster <= 0) {
    std::cerr << "[ERROR] The number of iteration must be greater than zero."
              << std::endl;
    return EXIT_FAILURE;
  }
  double beta = option.find(OPT_BETA) != option.end() ?
    atof(option[OPT_BETA].c_str()) : DEFAULT_BETA;
  if (num_cluster <= 0) {
    std::cerr << "[ERROR] Beta value must be greater than zero."
              << std::endl;
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

  bayon::PLSI plsi(num_cluster, beta, DEFAULT_SEED);
  read_documents(ifs_doc, plsi, veckey, docid2str, veckey2str, str2veckey);
  plsi.init_prob();
  plsi.em(num_iter);
  bool normalize = (option.find(OPT_NORMALIZE) != option.end()) ? true : false;
  plsi.show_membership(docid2str, normalize);

  return EXIT_SUCCESS;
}

/* show usage */
static void usage(std::string progname) {
  std::cerr
    << progname << ": Clustering tool by probabilistic latent semantic indexing"
    << std::endl
    << "Usage:" << std::endl
    << " % " << progname << " -n num [options] file" << std::endl
    << "    -n, --number=num      the number of clusters" << std::endl
    << "    -i, --iter=num        the number of iteration (default:"
    << DEFAULT_NUM_ITER << ")" << std::endl
    << "    -b, --beta=double     the parameter of tempered EM (default:"
    << DEFAULT_BETA << ")" << std::endl
    << "    --normalize           normalize output probabilities" << std::endl;
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
    case OPT_NORMALIZE:
      option[OPT_NORMALIZE] = DUMMY_OPTARG;
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

