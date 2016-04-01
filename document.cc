#include <math.h>
#include <assert.h>


#include "utils.h"
#include "document.h"
#include "author.h"
#include "table.h"

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
// WordUtils
// =======================================================================

void WordUtils::UpdateAuthorFromWord(Word* word,  
																		int update) {
	int author_id = word->getAuthorId();
	if (author_id == -1) return;
	assert(author_id != -1);

	AllAuthors& all_authors = AllAuthors::GetInstance();
	Author* author = all_authors.getMutableAuthor(author_id);
	author->updateWordCount(update);

	if (update == -1) {	
		author->removeWord(word);
	} 

	if (update == 1) {
		author->addWord(word);
	}

	UpdateTableFromWord(word, update);

	// Set removed word's table and topic nullptr.
	if (update == -1) {
		Table* table = word->getMutableTable();		
		table->setTopic(nullptr);
		word->setTable(nullptr);
	}
}

void WordUtils::UpdateTableFromWord(Word* word,
																		int update) {
	Table* table = word->getMutableTable();
	if (table == nullptr) return;

	table->updateWordCount(update);
	table->updateWordCount(word->getId(), update);

	TableUtils::UpdateTopicFromTable(table, word->getId(), update);
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
	if (permute_words == 1) {
		PermuteWords(document);
	}

	int words = document->getWords();
	for (int i = 0; i < words; i++) {
		Word* word = document->getMutableWord(i);
		SampleAuthorForWord(document, word, remove);
	}
}

void DocumentUtils::SampleAuthorForWord(Document* document,
																				Word* word,
																				bool remove) {

	int authors = document->getAuthors();
	assert(authors > 0);
	vector<double> log_pr(authors, log(1.0 / authors));
	int sample_index = Utils::SampleFromLogPr(log_pr); 
	int sample_author = document->getAuthor(sample_index);
	int old_author = word->getAuthorId();
	if (sample_author != old_author) {
		WordUtils::UpdateAuthorFromWord(word, -1);
		word->setAuthorId(sample_author);
		word->setTable(nullptr);
		WordUtils::UpdateAuthorFromWord(word, 1);
	}
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