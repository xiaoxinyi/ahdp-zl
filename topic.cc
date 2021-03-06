#include <math.h>
#include <assert.h>

#include <iostream>
#include <iomanip>

#include "utils.h"
#include "topic.h"
#include "table.h"

namespace ahdp {

// =======================================================================
// Topic
// =======================================================================

Topic::Topic(int corpus_word_no)
		: corpus_word_no_(corpus_word_no),
			word_count_(0),
			table_count_(0),
			word_counts_(corpus_word_no, 0) {
}

double Topic::getLogWordPr(int word_id) {
	double eta = AllTopics::GetInstance().getEta();
	double log_word_pr = log((word_counts_[word_id] + eta) / 
											    (word_count_ + eta * corpus_word_no_));
	return log_word_pr;
}

double Topic::getLgamWordEta(int word_id) {
	double eta = AllTopics::GetInstance().getEta();
	int word_count = word_counts_[word_id]; 
		
	double lgam_word_eta = gsl_sf_lngamma(word_count + eta);
	return lgam_word_eta;

}



// =======================================================================
// TopicUtils
// =======================================================================

double TopicUtils::EtaScore(Topic* topic, double eta) {
	double score = 0.0;
	double lgam_eta = gsl_sf_lngamma(eta);
	int corpus_word_no = topic->getCorpusWordNo();
	int topic_words = 0;
	
	score += gsl_sf_lngamma(corpus_word_no * eta);
	for (int i = 0; i < corpus_word_no; i++) {
		int word_count = topic->getWordCount(i);
		if (word_count > 0) {
			topic_words += word_count;
			score += gsl_sf_lngamma(word_count + eta) - lgam_eta;
		}
	}
	score -= gsl_sf_lngamma(corpus_word_no * eta + topic_words);

	return score;
}

void TopicUtils::PrintTopicInfo(Topic* topic) {
	cout << "topic address : " << topic << endl;
	cout << "topic table count : " << topic->getTableCount() << endl;
	cout << "word No in topic : " << topic->getWordCount() << endl;
	int corpus_word_no = topic->getCorpusWordNo();
	cout << "word id : count" << endl;
	for (int i = 0; i < corpus_word_no; i++) {
		cout << i << " : " << topic->getWordCount(i) << endl;
	}
	cout << "=======================" << endl;
	cout << endl;
}

void TopicUtils::SaveTopic(Topic* topic, 
													 ofstream& ofs,
													 ofstream& ofs_counts) {
  ofs.precision(10);
  ofs << std::right;
  for (int i = 0; i < topic->getCorpusWordNo(); i++) {
    ofs << exp(topic->getLogWordPr(i)) << " ";
    ofs_counts << topic->getWordCount(i) << " ";
  }
  ofs << endl;
  ofs_counts << endl;
}

// =======================================================================
// TopicTableUtils
// =======================================================================

double TopicTableUtils::LogGammaRatio(Table* table,
											 								Topic* topic,
											 								const vector<int>& word_ids,
                                      const vector<int>& counts) {
	double log_gamma_ratio = 0.0;
	Topic* old_topic = table->getMutableTopic();

	double eta = AllTopics::GetInstance().getEta();
	int topic_words = topic->getWordCount();
	int table_words = table->getWordCount();
	int corpus_word_no = topic->getCorpusWordNo();

	if (old_topic == topic) {
		log_gamma_ratio += gsl_sf_lngamma(corpus_word_no * eta + topic_words - table_words) -
										 gsl_sf_lngamma(corpus_word_no * eta + topic_words);

		for (int i = 0; i < (int)word_ids.size(); i++) {
			int word_id = word_ids[i];
			int word_count = topic->getWordCount(word_id);
			log_gamma_ratio += topic->getLgamWordEta(word_id) -
												 gsl_sf_lngamma(word_count + eta - counts[i]); 
		}
	} else {
		log_gamma_ratio += gsl_sf_lngamma(corpus_word_no * eta + topic_words) -
										 gsl_sf_lngamma(corpus_word_no * eta + topic_words + table_words);

		for (int i = 0; i < (int)word_ids.size(); i++) {
			int word_id = word_ids[i];
			int word_count = topic->getWordCount(word_id);
			log_gamma_ratio += gsl_sf_lngamma(word_count + eta + counts[i]) - 
												 topic->getLgamWordEta(word_id);  
		}
	}

	return log_gamma_ratio;
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

unordered_map<Topic*, int> AllTopics::getTopicIds() {
	unordered_map<Topic*, int> m;
	int topics = topic_ptrs_.size();
	for (int i = 0; i < topics; i++) {
		Topic* topic = topic_ptrs_[i];
		m[topic] = i;
	}
	return m;
}

// =======================================================================
// AllTopicsUtils 
// =======================================================================

double AllTopicsUtils::GammaScore(double gamma) {
	double score = 0.0;
	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();
	int table_total = 0;

	for (int i = 0; i < topics; i++) {
		Topic* topic = all_topics.getMutableTopic(i);
		int tables = topic->getTableCount();
		score += gsl_sf_lngamma(tables);
		table_total += tables; 
	}

	score += topics * log(gamma) +
					 gsl_sf_lngamma(gamma) - 
					 gsl_sf_lngamma(table_total + gamma);

	return score;
}

double AllTopicsUtils::EtaScore() {
	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();
	double eta = all_topics.getEta();

	double score = 0.0;
	for (int i = 0; i < topics; i++) {
		Topic* topic = all_topics.getMutableTopic(i);
		score += TopicUtils::EtaScore(topic, eta);
	}

	return score;
}

void AllTopicsUtils::PrintTopicsInfo() {
	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();
	for (int i = 0; i < topics; i++) {
		Topic* topic = all_topics.getMutableTopic(i);
		TopicUtils::PrintTopicInfo(topic);
	}
}

void AllTopicsUtils::SaveTopics(const string& filename_topics,
																const string& filename_topics_counts) {
	ofstream ofs(filename_topics);
	ofstream ofs_counts(filename_topics_counts);

	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();
	for (int i = 0; i < topics; i++) {
		Topic* topic = all_topics.getMutableTopic(i);
		TopicUtils::SaveTopic(topic, ofs, ofs_counts);
	}

	ofs.close();
	ofs_counts.close();
}

int AllTopicsUtils::GetWordNo() {
	AllTopics& all_topics = AllTopics::GetInstance();
	int total = 0;
	int topics = all_topics.getTopics();
	for (int i = 0; i < topics; i++) {
		total += all_topics.getMutableTopic(i)->getWordCount();
	}
	return total;
}

} // ahdp