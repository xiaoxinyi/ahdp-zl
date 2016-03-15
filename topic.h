#ifndef TOPIC_H_
#define TOPIC_H_

#include <vector>

#include <gsl/gsl_sf.h>

using std::vector;

namespace ahdp {

// The topic contains the corpus word number,
// the counts of each word, the number of tables,
// word count in this topic.
class Topic {
public:
	Topic(int corpus_word_no);

	int getWordCount() const { return word_count_; }
	void setWordCount(const int& word_count) { word_count_ = word_count; }
	void updateWordCount(int update) { word_count_ += update; }

	int getCorpusWordNo() const { return corpus_word_no_; }
	void setCorpusWordNO(int corpus_word_no) { corpus_word_no_ = corpus_word_no; }

	void updateWordCounts(int word_id, int update) {
		word_counts_[word_id] += update;
	}
	int getWordCount(int word_id) const { return word_counts_[word_id]; }

	int getTableCount() const { return table_count_; }
	void setTableCount(int table_count) { table_count = table_count_; }
	void updateTableCount(int update) { table_count_ += update; }

private:
	// Corpus word number - vocabulary size.
	int corpus_word_no_;

	// Word count in the topic.
	int word_count_;

	// The count of tables pointing this topic.
	int table_count_;

	// Counts of each word assigned to this topic.
	vector<int> word_counts_;
	
};


// AllTopics store all the topics globally.
// This class provides functionality of 
// adding new topic, removing topic,
// compacting topics after sampling.
class AllTopics {
public:
	~AllTopics();

	// Declare but no implemention.
	AllTopics(const AllTopics& from) = delete;
	AllTopics& operator=(const AllTopics& from) = delete;

	// Sigleton instance of AllTopics.
	static AllTopics& GetInstance();

	int getTopics() const { return topic_ptrs_.size(); }

	void addNewTopic(int corpus_word_no);
	void removeTopic(Topic* topic);
	void removeTopic(int pos);
	void removeLastTopic();
	void compactTopics();
	
	Topic* getMutableTopic(int i) {
		return topic_ptrs_[i];
	}

	int getEta() const { return eta_; }
	void setEta(const int& eta) { eta_ = eta; }

private:
	// All topics.
	vector<Topic*> topic_ptrs_;

	// Dirichlet parameter for word in topic.
	double eta_; 

	// Private constructor.
	AllTopics() { }
	
};


} // ahdp
#endif