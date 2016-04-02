#include <assert.h>

#include <iostream>

#include "state.h"
#include "author.h"
#include "table.h"

namespace ahdp {

// =======================================================================
// StateUtils
// =======================================================================


map<Topic*, int> StateUtils::topic_word_counts_;
map<Topic*, int> StateUtils::topic_table_counts_;

bool StateUtils::CheckSumForTopic(Topic* topic) {
	int word_count = 0;
	int corpus_word_no = topic->getCorpusWordNo();

	cout << "topic table count sum : " << topic_table_counts_[topic] << endl;
	cout << "topic table count : " << topic->getTableCount() << endl;

	for (int i = 0; i < corpus_word_no; i++) {
		word_count += topic->getWordCount(i);
	}

	cout << "topic word count sum : " << word_count << endl;
	cout << "topic word count : " << topic->getWordCount() << endl;
	cout << "topic word count by tables : " << topic_word_counts_[topic] << endl;

	return word_count == topic->getWordCount() &&
	       word_count == topic_word_counts_[topic] && 
	       topic_table_counts_[topic] == topic->getTableCount();
}


bool StateUtils::CheckSumForAllTopics() {
	bool res = true;
	AllTopics& all_topics = AllTopics::GetInstance();

	int topics = all_topics.getTopics();
	for (int i = 0; i < topics; i++) {
		cout << "check topic " << i << " :" << endl;
		Topic* topic = all_topics.getMutableTopic(i);

		res = CheckSumForTopic(topic) && res;
	}

	return res;
}

bool StateUtils::CheckSumForAuthor(Author* author) {
	int tables = author->getTables();
	int word_count = 0;
	cout << "author " << author->getId() << " has "
		 	 << tables << " tables :" << endl;

	for (int i = 0; i < tables; i++) {
		cout << "table " << i << " :" << endl;
		Table* table = author->getMutableTable(i);
		CheckSumForTable(table);
		word_count += table->getWordCount();
	}

	cout << "author " << author->getId() 
			 << " word count sum : " << word_count << endl;
	cout << "author " << author->getId() 
	     << " word count : " << author->getWordCount() << endl;

	return word_count == author->getWordCount();
}

bool StateUtils::CheckSumForAllAuthors() {
	bool res = true;
	AllAuthors& all_authors = AllAuthors::GetInstance();
	int authors = all_authors.getAuthors();

	for (int i = 0; i < authors; i++) {
		Author* author = all_authors.getMutableAuthor(i);
		res = CheckSumForAuthor(author) && res;
	}
	return res;
}

bool StateUtils::CheckSumForTable(Table* table) {
	int word_count = 0;

	Topic* topic = table->getMutableTopic();
	UpdateTopicTableCounts(topic, 1);

	auto& word_counts = table->getWordCounts();
	for (auto& p : word_counts) {
		word_count += p.second;
	}
	UpdateTopicWordCounts(topic, word_count);

	cout << "table word count sum : " << word_count << endl;
	cout << "table word count : " << table->getWordCount() << endl;

	return word_count == table->getWordCount();
}

void StateUtils::UpdateTopicWordCounts(Topic* topic, int update) {
	auto found = topic_word_counts_.find(topic);
	if (found == topic_word_counts_.end()) {
		topic_word_counts_[topic] = update;
	} else {
		topic_word_counts_[topic] += update;
	}
}

void StateUtils::UpdateTopicTableCounts(Topic* topic, int update) {
	auto found = topic_table_counts_.find(topic);
	if (found == topic_table_counts_.end()) {
		topic_table_counts_[topic] = update;
	} else {
		topic_table_counts_[topic] += update;
	}
}

bool StateUtils::CheckSum() {
	bool res = true;
	res = CheckSumForAllAuthors();
	res = CheckSumForAllTopics() && res;
	topic_table_counts_.erase(begin(topic_table_counts_), end(topic_table_counts_));
	topic_word_counts_.erase(begin(topic_word_counts_), end(topic_word_counts_));
	if (res) {
		cout << "status ok." << endl;
	} else {
		cout << "status bad." << endl;
	}
	return res;
}

}  // namespace ahdp