#include <math.h>
#include <assert.h>


#include "utils.h"
#include "document.h"
#include "author.h"

namespace ahdp {

// =======================================================================
// Word
// =======================================================================

Word::Word(int id) 
		: id_(id),
			author_id_(-1),
			table_(nullptr) {

}

// =======================================================================
// Document
// =======================================================================

Document::Document(int id)
		: id_(id) {

}

Document::~Document() {
	for (auto& word : words_) {
		if (word != nullptr) {
			delete word;
			word = nullptr;
		}
	}
}


Document& Document::operator=(Document&& from) {
	if (this == &from) {
		return *this;
	}
	for (auto& word : words_) {
		if (word != nullptr) {
			delete word;
			word = nullptr;
		}
	}
	words_.resize(0);
	
	id_ = from.id_;
	words_ = move(from.words_);

	return *this;

}


vector<Author*> Document::getMutableAuthors() const {
	vector<Author*> authors;
	AllAuthors& all_authors = AllAuthors::GetInstance();
	authors.reserve(authors_.size());

	for (auto& author_id : authors_) {
		Author* author = all_authors.getMutableAuthor(author_id);
		authors.push_back(author);
	}

	return authors;
}

// =======================================================================
// DocumentUtils
// =======================================================================

void DocumentUtils::SampleAuthors(Document* document,
													 int permute_words,
											     bool remove) {

}


void DocumentUtils::PermuteWords(Document* document) {
  int size = document->getWords();
  vector<Word*> permuted_words;

  // Permute the values in perm.
  // These values correspond to the indices of the words in the
  // word vector of the document.
  gsl_permutation* perm = gsl_permutation_calloc(size);
  Utils::Shuffle(perm, size);
  int perm_size = perm->size;
  assert(size == perm_size);

  for (int i = 0; i < perm_size; i++) {
  	Word* word = document->getMutableWord(i);
    permuted_words.push_back(word);
  }

  document->setWords(move(permuted_words));

  gsl_permutation_free(perm);
}



} // ahdp