#ifndef TOPIC_H_
#define TOPIC_H_

#include <vector>
#include <fstream>
#include <string>
#include <gsl/gsl_sf.h>
#include <unordered_map>

using namespace std;

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

	void updateWordCount(int word_id, int update) {
		word_counts_[word_id] += update;
	}
	int getWordCount(int word_id) const { return word_counts_[word_id]; }

	int getTableCount() const { return table_count_; }
	void setTableCount(int table_count) { table_count_ = table_count; }
	void updateTableCount(int update) { table_count_ += update; }

	double getLogWordPr(int word_id);
	double getLgamWordEta(int word_id);
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


// The class provides functionality for computing
// eta score.
class TopicUtils {
public:
	// Computing eta score for given topic.
	static double EtaScore(Topic* topic, double eta);

	static void PrintTopicInfo(Topic* topic);

	static void SaveTopic(Topic* topic, 
												ofstream& ofs,
												ofstream& ofs_counts);

};


class Table;
// This class provides functionality for computing
// Log gamma ratio of each topic given a table.
class TopicTableUtils {
public:
	// Compute log gamma ratio for table given a topic.
	// table - given table.
	// topic - possible to be sampled topic.
	// word_ids - distinct word ids in table.
	// counts - corresponding counts.
	static double LogGammaRatio(Table* table,
											 				Topic* topic,
											 				const vector<int>& word_ids,
											 				const vector<int>& counts);
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

	unordered_map<Topic*, int> getTopicIds();

private:
	// All topics.
	vector<Topic*> topic_ptrs_;

	// Dirichlet parameter for word in topic.
	double eta_; 

	// Private constructor.
	AllTopics() { topic_ptrs_.reserve(2000); }
	
};

// This class provides functionality for computing
// Gamma score, Eta scores.
class AllTopicsUtils {
public:
	// Compute gamma score.
	// gamma - first level dirichlet process parameter.
	static double GammaScore(double gamma);

	// Compute eta score.
	// eta - dirichlet distribution parameter of each topic.
	static double EtaScore();

	static void PrintTopicsInfo();

	static void SaveTopics(const string& filename_topics,
												 const string& filename_topics_counts);

	static int GetWordNo();
};


} // ahdp
#endif