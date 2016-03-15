#include <math.h>
#include <assert.h>

#include <iostream>

#include "utils.h"
#include "topic.h"

namespace ahdp {

// =======================================================================
// Topic
// =======================================================================

Topic::Topic(int corpus_word_no)
		: corpus_word_no_(corpus_word_no),
			word_count_(0),
			table_count_(0) {
}

// =======================================================================
// AllTopics
// =======================================================================

AllTopics::~AllTopics() {
	int size = topic_ptrs_.size();
	for (int i = 0; i < size; i++) {
		if (topic_ptrs_[i] != nullptr) {
			delete topic_ptrs_[i];
		}
	}
}

AllTopics& AllTopics::GetInstance() {
	static AllTopics instance;
	return instance;
}
	
void AllTopics::addNewTopic(int corpus_word_no) {
	Topic* topic = new Topic(corpus_word_no);
	topic_ptrs_.push_back(topic);
}

void AllTopics::removeTopic(Topic* topic) {
	removeFromVec(topic_ptrs_, topic);
} 

void AllTopics::removeTopic(int pos) {
	auto it = topic_ptrs_.begin() + pos;
	Topic* topic = *it;
	assert(topic->getTableCount() == 0);

	delete *it;
	topic = nullptr;
	*it = nullptr;
	topic_ptrs_.erase(it);
}


void AllTopics::removeLastTopic() {
	int size = topic_ptrs_.size();
	if (topic_ptrs_[size - 1] != nullptr) {
			delete topic_ptrs_[size - 1];		
	}
	topic_ptrs_[size - 1] = nullptr;
	topic_ptrs_.resize(size - 1);
}

void AllTopics::compactTopics() {
	int size = topic_ptrs_.size();
	for (int i = size - 1; i >=0; --i) {
		if (topic_ptrs_[i]->getTableCount() == 0) {
			removeTopic(i);
		}
	}
}
} // ahdp