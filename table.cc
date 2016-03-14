#include <assert.h>

#include <iostream>

#include "table.h"
#include "topic.h"
#include "utils.h"

namespace ahdp {

// =======================================================================
// Table
// =======================================================================

	Table::Table()
		: word_count_(0) {
	topic_ = nullptr;
}

Table::Table(int word_count)
		: word_count_(word_count) {
	topic_ = nullptr;
}

int Table::getWordCount(int word_id) {
	auto it = word_counts_.find(word_id);
	if (it == word_counts_.end()) {
		return 0;
	} else {
		return word_counts_[word_id];
	}
}

void Table::updateWordCount(int word_id, int update) {
	auto it = word_counts_.find(word_id);
	if (it == word_counts_.end()) {
		word_counts_[word_id] = update;
	} else {
		word_counts_[word_id] += update;
	}
	assert(word_counts_[word_id] >= 0);
	if (word_counts_[word_id] == 0) {
		word_counts_.erase(word_id);
	}
	return;
}

} // ahdp