#include <iostream>

#include "gibbs.h"
#include "author.h"
#include "state.h"

using ahdp::GibbsSampler;
using ahdp::GibbsState;
using ahdp::AllTopicsUtils;
using ahdp::AllAuthorsUtils;
using namespace ahdp;

#define MAX_ITERATIONS 100
const string DIR = "~/code/cplusplus/atm-zl/";
const string FILENAME_TOPICS = "result/train-topics-final.dat";
const string FILENAME_TOPICS_COUNTS = "result/train-topics-counts-final.dat";
const string FILENAME_AUTHOR_COUNTS = "result/train-author-counts-final.dat";


int main(int argc, char** argv) {
  if (argc == 4) {
    // The random number generator seed.
    // For testing an example seed is: t = 1147530551;
    long rng_seed = 458312327;
    // (void) time(&rng_seed);

    std::string filename_corpus = argv[1];
    std::string filename_authors = argv[2];
    std::string filename_settings = argv[3];
    ahdp::GibbsState* gibbs_state = ahdp::GibbsSampler::InitGibbsStateRep(
        filename_corpus, filename_authors,
        filename_settings, rng_seed);
    StateUtils::CheckSum();
    
    GibbsSampler::RandomIterateGibbsState(gibbs_state);

    for (int i = 0; i < MAX_ITERATIONS; i++) {
      StateUtils::CheckSum();
      ahdp::GibbsSampler::IterateGibbsState(gibbs_state);
    }
    StateUtils::CheckSum();
    AllTopicsUtils::SaveTopics(FILENAME_TOPICS, FILENAME_TOPICS_COUNTS);
    AllAuthorsUtils::SaveAuthors(FILENAME_AUTHOR_COUNTS);

    delete gibbs_state;
  } else {
    cout << "Arguments: "
        "(1) corpus filename "
        "(2) author filename "
        "(3) settings filename" << endl;
  }
  return 0;
}

