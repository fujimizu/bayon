//
// LDA (Latent Dirichlet Allocation)
//
// Reference: http://www.arbylon.net/projects/LdaGibbsSampler.java
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
} lda_options;

typedef std::map<lda_options, std::string> Option;
typedef bayon::HashMap<std::string, double>::type Feature;
typedef bayon::HashMap<bayon::DocumentId, std::string>::type DocId2Str;
typedef bayon::HashMap<bayon::VecKey, std::string>::type VecKey2Str;
typedef bayon::HashMap<std::string, bayon::VecKey>::type Str2VecKey;


/********************************************************************
 * constants
 *******************************************************************/
const std::string DUMMY_OPTARG  = "dummy";
const size_t DEFAULT_THIN_INTERVAL = 100;
const size_t DEFAULT_BURN_IN       = 2000;
const size_t DEFAULT_ITERATIONS    = 10000;
const size_t DEFAULT_SAMPLE_LAG    = 10;
const unsigned int DEFAULT_SEED    = 12345;


/********************************************************************
 * global variables
 *******************************************************************/
struct option longopts[] = {
  {"number",    required_argument, NULL, OPT_NUMBER   },
  {"iter",      required_argument, NULL, OPT_ITER     },
  {0, 0, 0, 0}
};


/********************************************************************
 * class
 *******************************************************************/
namespace bayon {

class LDA {
 public:
  typedef HashMap<VecKey, int>::type WordAssign;

 private:
  std::vector<Document *> documents_;
  size_t num_doc_;
  size_t num_word_;
  size_t num_topic_;
  double alpha_;
  double beta_;
  WordAssign **z_; // topic assignments for each word
  int **nw_;       // nw[i][j] number of instances of word i assigned to topic j
  int **nd_;       // nd[i][j] number of words in document i assigned to topic j
  int *nwsum_;     // nwsum[j] total number of words assigned to topic j
  int *ndsum_;     // ndsum[i] total number of words in document i
  double **thetasum_; // cumulative statistics of theta
  double **phisum_;   // cumulative statistics of phi
  size_t numstats_;   // size of statistics
  unsigned int seed_;

  size_t thin_interval_; // sampling lag
  size_t burn_in_;       // burn-in period
  size_t iterations_;    // max iterations
  size_t sample_lag_;    // sample lag
  size_t dispcol_;

  void initialize(size_t num_topic) {
    nw_    = new int*[num_word_];
    nd_    = new int*[num_doc_];
    nwsum_ = new int[num_topic_];
    ndsum_ = new int[num_doc_];
    z_     = new WordAssign*[num_doc_];

    for (size_t it = 0; it < num_topic_; it++) nwsum_[it] = 0;
    for (size_t iw = 0; iw < num_word_; iw++) {
      nw_[iw] = new int[num_topic_];
      for (size_t it = 0; it < num_topic_; it++) nw_[iw][it] = 0;
    }
    for (size_t id = 0; id < num_doc_; id++) {
      nd_[id] = new int[num_topic_];
      for (size_t it = 0; it < num_topic_; it++) nd_[id][it] = 0;

      z_[id] = new WordAssign;
      init_hash_map(bayon::VECTOR_EMPTY_KEY, *z_[id]);
      double sum = 0.0;
      VecHashMap *hmap = documents_[id]->feature()->hash_map();
      for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
        int topic = static_cast<int>(
          static_cast<double>(rand()) / RAND_MAX * num_topic_);
          //static_cast<double>(myrand(&seed_)) / RAND_MAX * num_topic_);
        if (topic == static_cast<int>(num_topic_)) topic--;
        (*z_[id])[it->first] = topic;
        nw_[it->first][topic] += it->second;
        nd_[id][topic] += it->second;
        nwsum_[topic] += it->second;
        sum += it->second;
//        nw_[it->first][topic]++;
//        nd_[id][topic]++;
//        nwsum_[topic]++;
      }
      ndsum_[id] = sum;
//      ndsum_[id] = documents_[id]->feature()->size();
    }

    // initialize sampler statistics
    if (sample_lag_ > 0) {
      thetasum_ = new double*[num_doc_];
      for (size_t id = 0; id < num_doc_; id++) {
        thetasum_[id] = new double[num_topic_];
        for (size_t it = 0; it < num_topic_; it++) thetasum_[id][it] = 0.0;
      }
      phisum_   = new double*[num_topic_];
      for (size_t it = 0; it < num_topic_; it++) {
        phisum_[it] = new double[num_word_];
        for (size_t iw = 0; iw < num_word_; iw++) phisum_[it][iw] = 0.0;
      }
      numstats_ = 0;
    }
  }

  int sample_full_conditional2(size_t id, VecKey word, VecValue point) {
    int topic = (*z_[id])[word];
    for (size_t i = 0; i < point; i++) {
      nw_[word][topic]--;
      nd_[id][topic]--;
      nwsum_[topic]--;
      ndsum_[id]--;

      double p[num_topic_];
      for (size_t it = 0; it < num_topic_; it++) {
        p[it] = (nw_[word][it] + beta_) / (nwsum_[it] + num_word_ * beta_)
                * (nd_[id][it] + alpha_) / (ndsum_[id] + num_topic_ * alpha_);
      }
      for (size_t it = 1; it < num_topic_; it++) {
        p[it] += p[it-1];
      }
      //double u = static_cast<double>(myrand(&seed_)) / RAND_MAX
      double u = static_cast<double>(rand()) / RAND_MAX
                 * p[num_topic_-1];
      for (size_t it = 0; it < num_topic_; it++) {
        if (u < p[it]) {
          topic = static_cast<int>(it);
          break;
        }
      }
      nw_[word][topic]++;
      nd_[id][topic]++;
      nwsum_[topic]++;
      ndsum_[id]++;
    }
    return topic;
  }

  int sample_full_conditional(size_t id, VecKey word, VecValue point) {
    int topic = (*z_[id])[word];
    nw_[word][topic] -= point;
    nd_[id][topic]   -= point;
    nwsum_[topic]    -= point;
    ndsum_[id]       -= point;
    /*
    nw_[word][topic]--;
    nd_[id][topic]--;
    nwsum_[topic]--;
    ndsum_[id]--;
    */

    double p[num_topic_];
    for (size_t it = 0; it < num_topic_; it++) {
//      p[it] = (nw_[word][it] + beta_) / (nwsum_[it] + beta_)
//              * (nd_[id][it] + alpha_) / (ndsum_[id] + alpha_ - point);
      p[it] = (nw_[word][it] + beta_) / (nwsum_[it] + num_word_ * beta_)
              * (nd_[id][it] + alpha_) / (ndsum_[id] + num_topic_ * alpha_);
    }
    for (size_t it = 1; it < num_topic_; it++) {
      p[it] += p[it-1];
    }
    //double u = static_cast<double>(myrand(&seed_)) / RAND_MAX
    double u = static_cast<double>(rand()) / RAND_MAX
               * p[num_topic_-1];
    for (size_t it = 0; it < num_topic_; it++) {
      if (u < p[it]) {
        topic = static_cast<int>(it);
        break;
      }
    }

    nw_[word][topic] += point;
    nd_[id][topic]   += point;
    nwsum_[topic]    += point;
    ndsum_[id]       += point;
    /*  
    nw_[word][topic]++;
    nd_[id][topic]++;
    nwsum_[topic]++;
    ndsum_[id]++;
    */
    return topic;
  }

  void update_params() {
    for (size_t id = 0; id < num_doc_; id++) {
      for (size_t it = 0; it < num_topic_; it++) {
        thetasum_[id][it] += (nd_[id][it] + alpha_)
                             / (ndsum_[id] + num_topic_ * alpha_);
      }
    }
    for (size_t it = 0; it < num_topic_; it++) {
      for (size_t iw = 0; iw < num_word_; iw++) {
        phisum_[it][iw] += (nw_[iw][it] + beta_)
                           / (nwsum_[it] + num_word_ * beta_);
      }
    }
    numstats_++;
  }

 public:
  LDA(size_t thin_interval, size_t burn_in, size_t iterations,
      size_t sample_lag)
    : num_doc_(0), num_word_(0), num_topic_(0), alpha_(0), beta_(0),
      z_(NULL), nw_(NULL), nd_(NULL), nwsum_(NULL), ndsum_(NULL),
      thetasum_(NULL), phisum_(NULL), numstats_(0), seed_(DEFAULT_SEED), 
      thin_interval_(thin_interval), burn_in_(burn_in),
      iterations_(iterations), sample_lag_(sample_lag), dispcol_(0) { }

  ~LDA() {
    for (size_t id = 0; id < documents_.size(); id++) {
      delete documents_[id];
      if (nd_ && nd_[id])             delete [] nd_[id];
      if (z_ && z_[id])               delete z_[id];
      if (thetasum_ && thetasum_[id]) delete [] thetasum_[id];
    }
    for (size_t iw = 0; iw < num_word_; iw++) {
      if (nw_ && nw_[iw])         delete [] nw_[iw];
    }
    for (size_t it = 0; it < num_topic_; it++) {
      if (phisum_ && phisum_[it]) delete [] phisum_[it];
    }
    if (nw_)       delete [] nw_;
    if (nd_)       delete [] nd_;
    if (nwsum_)    delete [] nwsum_;
    if (ndsum_)    delete [] ndsum_;
    if (z_)        delete [] z_;
    if (thetasum_) delete [] thetasum_;
    if (phisum_)   delete [] phisum_;
  }

  void gibbs(size_t num_topic, double alpha, double beta) {
    num_topic_ = num_topic;
    alpha_ = alpha;
    beta_  = beta;
    initialize(num_topic_);

    for (size_t i = 0; i < iterations_; i++) {
      for (size_t id = 0; id < num_doc_; id++) {
        VecHashMap *hmap = documents_[id]->feature()->hash_map();
        for (VecHashMap::iterator it = hmap->begin(); it != hmap->end(); ++it) {
          int topic = sample_full_conditional(id, it->first, it->second);
          (*z_[id])[it->first] = topic;
        }
      }

/*
      if ((i < burn_in_) && (i % thin_interval_ == 0)) {
        std::cout << "B";
        dispcol_++;
      }
      if ((i > burn_in_) && (i % thin_interval_ == 0)) {
        std::cout << "S";
        dispcol_++;
      }
*/
      if ((i > burn_in_) && (sample_lag_ > 0) && (i % sample_lag_ == 0)) {
        update_params();
//        std::cout << "|";
        if (i % thin_interval_ != 0) dispcol_++;
      }
/*
      if (dispcol_ >= 100) {
        std::cout << std::endl;
        dispcol_ = 0;
      }
*/
    }
  }

  void print_theta(
    const HashMap<DocumentId, std::string>::type &docid2str) const {
    HashMap<DocumentId, std::string>::type::const_iterator itr;
    for (size_t id = 0; id < num_doc_; id++) {
      itr = docid2str.find(documents_[id]->id());
      if (itr != docid2str.end()) std::cout << itr->second;
      else                        std::cout << documents_[id]->id();

      for (size_t it = 0; it < num_topic_; it++) {
        std::cout << "\t";
        double val;
        if (sample_lag_ > 0) {
          val = thetasum_[id][it] / numstats_;
        } else {
          val = (nd_[id][it] + alpha_) / (ndsum_[id] + num_topic_ * alpha_);
        }
        if (isnan(val)) val = 0;
        std::cout << val;
      }
      std::cout << std::endl;
    }
  }

  void print_phi(
    const HashMap<VecKey, std::string>::type &veckey2str) const {
    HashMap<VecKey, std::string>::type::const_iterator itr;
    for (size_t it = 0; it < num_topic_; it++) {
      std::cout << it;
      for (size_t iw = 0; iw < num_word_; iw++) {
        std::cout << "\t";
        itr = veckey2str.find(iw);
        if (itr != veckey2str.end()) std::cout << itr->second;
        else                         std::cout << iw;

        std::cout << "\t";
        if (sample_lag_ > 0) {
          std::cout << phisum_[it][iw] / numstats_;
        } else {
          std::cout << (nw_[iw][it] + beta_)
                       / (nwsum_[it] + num_word_ * beta_);
        }
      }
      std::cout << std::endl;
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
};

} /* namespace bayon */


/********************************************************************
 * function prototypes
 *******************************************************************/
int main(int argc, char **argv);
static void usage(std::string progname);
static int parse_options(int argc, char **argv, Option &option);
static size_t parse_tsv(std::string &tsv, Feature &feature);
static size_t read_documents(std::ifstream &ifs, bayon::LDA &lda,
                      bayon::VecKey &veckey, DocId2Str &docid2str,
                      VecKey2Str &veckey2str, Str2VecKey &str2veckey);

/* main function */
int main(int argc, char **argv) {
  srand(time(NULL));
  std::string progname(argv[0]);
  Option option;
  int optind = parse_options(argc, argv, option);
  argc -= optind;
  argv += optind;
  if (argc != 1 || option.find(OPT_NUMBER) == option.end()) {
    usage(progname);
    return EXIT_FAILURE;
  }

  size_t num_topic = static_cast<size_t>(atoi(option[OPT_NUMBER].c_str()));
  if (num_topic <= 0) {
    std::cerr << "[ERROR] The number of output cluster must be greater than zero."
              << std::endl;
    return EXIT_FAILURE;
  }
  size_t num_iter = option.find(OPT_ITER) != option.end() ?
    static_cast<size_t>(atoi(option[OPT_ITER].c_str())) : DEFAULT_ITERATIONS;
  if (num_topic <= 0) {
    std::cerr << "[ERROR] The number of iteration must be greater than zero."
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

  bayon::LDA lda(DEFAULT_THIN_INTERVAL, DEFAULT_BURN_IN,
                 num_iter, DEFAULT_SAMPLE_LAG);
  read_documents(ifs_doc, lda, veckey, docid2str, veckey2str, str2veckey);
  double alpha = 2;
  double beta = 0.5;
  lda.gibbs(num_topic, alpha, beta);
//  std::cout << std::endl << std::endl;
  lda.print_theta(docid2str);
//  std::cout << std::endl;
//  lda.print_phi(veckey2str);

  return EXIT_SUCCESS;
}

/* show usage */
static void usage(std::string progname) {
  std::cerr
    << progname << ": Clustering tool by Latent Dirichlet Allocation"
    << std::endl
    << "Usage:" << std::endl
    << " % " << progname << " -n num [options] file" << std::endl
    << "    -n, --number=num  the number of clusters" << std::endl
    << "    -i, --iter=num    the number of iteration (default:"
    << DEFAULT_ITERATIONS << ")" << std::endl;
}

/* parse command line options */
static int parse_options(int argc, char **argv, Option &option) {
  int opt;
  extern char *optarg;
  extern int optind;
  while ((opt = getopt_long(argc, argv, "n:i:", longopts, NULL))
         != -1) {
    switch (opt) {
    case OPT_NUMBER:
      option[OPT_NUMBER] = optarg;
      break;
    case OPT_ITER:
      option[OPT_ITER] = optarg;
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

/* read input file and add documents to LDA object */
static size_t read_documents(std::ifstream &ifs, bayon::LDA &lda,
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
      lda.add_document(doc);
    }
  }
  return docid;
}

