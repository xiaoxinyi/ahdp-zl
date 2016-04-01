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

// =======================================================================
// TableUtils
// =======================================================================

void TableUtils::UpdateTopicFromTable(Table* table,
																			int word_id,
																			int update) {
	if (table == nullptr) return;
	Topic* topic = table->getMutableTopic();
	if (topic == nullptr) return;
	if (table->getWordCount() == 1 && update == 1) {
		topic->updateTableCount(1);
	}

	if (table->getWordCount() == 0 and update == -1) {
		topic->updateTableCount(-1);
	}

	topic->updateWordCount(update);
	topic->updateWordCount(word_id, update);

}

void TableUtils::UpdateTopicFromTable(Table* table,
																			const vector<int>& word_ids,
																			const vector<int>& counts,
																	    int update) {
	int size = word_ids.size();
	Topic* topic = table->getMutableTopic();
	if (topic == nullptr) return;
	topic->updateTableCount(update);

	for (int i = 0; i < size; i++) {
		int word_id = word_ids[i];
		
		topic->updateWordCount(update * counts[i]);
		topic->updateWordCount(word_id, update * counts[i]);
	}

}

void TableUtils::GetWordsAndCounts(Table* table,
																	 vector<int>& word_ids,
																	 vector<int>& counts) {
	unordered_map<int, int>& m = table->getWordCounts();
	int size = m.size();

	word_ids.reserve(size);
	counts.reserve(size);

	for (auto p : m) {
		word_ids.push_back(p.first);
		counts.push_back(p.second);
	}
}


void TableUtils::SampleTopicForTable(Table* table, 
																		 double gamma,
																		 bool remove) {
	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();

	vector<int> word_ids;
	vector<int> counts;
	TableUtils::GetWordsAndCounts(table, word_ids, counts);
	
	assert(topics >= 1);
	int corpus_word_no = all_topics.getMutableTopic(0)->getCorpusWordNo();
	all_topics.addNewTopic(corpus_word_no);
	
	vector<double> log_pr(topics + 1, 0.0);
	for (int i = 0; i < topics; i++) {
		Topic* topic = all_topics.getMutableTopic(i);
		if (table->getMutableTopic() != topic) {
			log_pr[i] = TopicTableUtils::LogGammaRatio(table, topic, 
																								 word_ids, counts) +
									log(topic->getTableCount());
		} else {
			log_pr[i] = TopicTableUtils::LogGammaRatio(table, topic,
																								 word_ids, counts) +
									log(topic->getTableCount() - 1);
		}					
	}

	Topic* new_ = all_topics.getMutableTopic(topics);
	log_pr[topics] = log(gamma) + 
										TopicTableUtils::LogGammaRatio(
											table, new_, word_ids, counts);
			

	int sample_topic = Utils::SampleFromLogPr(log_pr);
	Topic* old_topic = table->getMutableTopic();
	Topic* new_topic = all_topics.getMutableTopic(sample_topic);

	if (old_topic != new_topic) {
		TableUtils::UpdateTopicFromTable(table, word_ids, counts, -1);	

		table->setTopic(new_topic);
		TableUtils::UpdateTopicFromTable(table, word_ids, counts, 1);
	}

	if (sample_topic != topics) {
		all_topics.removeLastTopic();
	}

}

} // ahdp