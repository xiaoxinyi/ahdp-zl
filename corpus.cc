#include <gsl/gsl_permutation.h>
#include <math.h>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "corpus.h"
#include "author.h"

#define BUF_SIZE 10000

namespace ahdp {

// =======================================================================
// Corpus
// =======================================================================

Corpus::Corpus()
    : gamma_(0.0),
      alpha_(0.0),
      word_no_(0),
      word_total_(0),
      author_no_(0) {
}

Corpus::Corpus(double gamma, double alpha)
    : gamma_(gamma),
      alpha_(alpha),
      word_no_(0), 
      word_total_(0),
      author_no_(0) {
}


// =======================================================================
// CorpusUtils
// =======================================================================

void CorpusUtils::ReadCorpus(
    const std::string& docs_filename,
    const std::string& authors_filename,
    Corpus* corpus) {

  ifstream infile(docs_filename.c_str());
  char buf[BUF_SIZE];

  ifstream authors_infile(authors_filename.c_str());
  char authors_buf[BUF_SIZE];

  int author_no = 0;
  int doc_no = 0;
  int word_no = 0;
  int total_word_count = 0;
  int words;

  while (infile.getline(buf, BUF_SIZE) && 
  			 authors_infile.getline(authors_buf, BUF_SIZE)) {
  	
  	istringstream s_line_author(authors_buf);
    std::vector<int> author_ids;
  	while (s_line_author.getline(authors_buf, BUF_SIZE, ' ')) {
  		int author_id = atoi(authors_buf);
  		if (author_id >= author_no) {
  			author_no = author_id + 1;
  		}
  		author_ids.push_back(author_id);
  	}

  	if (author_ids.empty()) {
  		continue;
  	}

    istringstream s_line(buf);
    // Consider each line at a time.
    int word_count_pos = 0;
    Document document(doc_no);
    // Set author ids
    document.setAuthors(author_ids);
    while (s_line.getline(buf, BUF_SIZE, ' ')) {
      if (word_count_pos == 0) {
        words = atoi(buf);
      } else {
        int word_id, word_count;
        istringstream s_word_count(buf);
        std::string str;
        getline(s_word_count, str, ':');
        word_id = atoi(str.c_str());
        getline(s_word_count, str, ':');
        word_count = atoi(str.c_str());
        total_word_count += word_count;
				for (int i = 0; i < word_count; i++) {
					document.addWord(word_id);
				}

        if (word_id >= word_no) {
          word_no = word_id + 1;
        }
      }
      word_count_pos++;
    }
    corpus->addDocument(move(document));
    doc_no += 1;
  }

  infile.close();

  vector<Author> authors;
  authors.reserve(author_no);

  AllAuthors& all_authors = AllAuthors::GetInstance();
  for (int i = 0; i < author_no; i++) {
    all_authors.addNewAuthor(i);
  }

  corpus->setWordNo(word_no);
  corpus->setAuthorNo(author_no);

  cout << "Number of documents in corpus: " << doc_no << endl;
  cout << "Number of authors in corpus: " << author_no << endl;
  cout << "Number of distinct words in corpus: " << word_no << endl;
  cout << "Number of words in corpus: " << total_word_count << endl;
}



void CorpusUtils::PermuteDocuments(Corpus* corpus) {
  int size = corpus->getDocuments();
  vector<Document> permuted_documents;

  // Permute the values in perm.
  // These values correspond to the indices of the documents in the
  // document vector of the corpus.
  gsl_permutation* perm = gsl_permutation_calloc(size);
  Utils::Shuffle(perm, size);
  int perm_size = perm->size;
  assert(size == perm_size);

  for (int i = 0; i < perm_size; i++) {
		Document document = move(*corpus->getMutableDocument((int)perm->data[i]));
    permuted_documents.emplace_back(move(document));
  }

  corpus->setDocuments(move(permuted_documents));

  gsl_permutation_free(perm);
}

double CorpusUtils::AlphaScore(Corpus* corpus) {
	double alpha = corpus->getAlpha();
	
	double score = 0.0;
	AllAuthors& all_authors = AllAuthors::GetInstance();
	int author_no = all_authors.getAuthors();
	for (int i = 0; i < author_no; i++) {
		Author* author = all_authors.getMutableAuthor(i);
		score += AuthorUtils::AlphaScore(author, alpha);
	}

	return score;
}

} // ahdp