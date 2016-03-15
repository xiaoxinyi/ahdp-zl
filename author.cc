#include <assert.h>
#include <math.h>

#include "utils.h"
#include "author.h"
#include "table.h"

namespace ahdp {

// =======================================================================
// Author
// =======================================================================

Author::Author(int id) 
		: id_(id),
		  word_count_(0) {
}

Author& Author::operator=(Author&& from) {
	if (this == &from) {
		return *this;
	}

	for (auto& table : tables_) {
		if (table != nullptr) {
			delete table;
			table = nullptr;
		}
	}
	words_.resize(0);
	word_count_ = from.word_count_;
	id_ = from.id_;
	
	words_ = move(from.words_);
	tables_ = move(from.tables_);

	return *this;
}

void Author::addNewTable() {
	Table* table = new Table;
	tables_.push_back(table);
} 

void Author::removeWord(Word* word) {
	auto found = find(begin(words_), end(words_), word);
	if (found == end(words_)) {
		return;
	} else {
		*found = nullptr;
		words_.erase(found);
	}
}

void Author::removeTable(int pos) {
	Table* table = tables_[pos];
	assert(table->getWordCount() == 0);
	assert(table->getWordCounts().size() == 0);

	auto it = tables_.begin() + pos;
	delete *it;
	*it = nullptr;
	tables_.erase(it);
}

Author::~Author() {
	for (auto& table : tables_) {
		if (table != nullptr) {
			delete table;
			table = nullptr;
		}
	}
}
// =======================================================================
// AllAuthors
// =======================================================================

AllAuthors& AllAuthors::GetInstance() {
	static AllAuthors instance;
	return instance;
}

} // ahdp