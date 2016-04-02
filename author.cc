#include <assert.h>
#include <math.h>

#include <iostream>
#include <map>

#include "utils.h"
#include "author.h"
#include "table.h"
#include "document.h"
#include "topic.h"
#include "state.h"


namespace ahdp {

// =======================================================================
// Author
// =======================================================================

Author::Author(int id) 
		: id_(id),
		  word_count_(0) {
}

Author::~Author() {
  for (auto& table : tables_) {
    if (table != nullptr) {
      delete table;
      table = nullptr;
    }
  }
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



// =======================================================================
// AuthorUtils
// =======================================================================

void AuthorUtils::SampleTables(Author* author,
																 bool remove,
																 double alpha,
											    			 double gamma,
											    			 int corpus_word_no) {


	int words = author->getWordCount();
	for (int i = 0; i < words; i++) {
		Word* word = author->getMutableWord(i);
		SampleTableForWord(author, word, remove, alpha, gamma, corpus_word_no);
	}
}

void AuthorUtils::SampleTableForWord(Author* author,
																 Word* word,
																 bool remove,
																 double alpha,
																 double gamma,
																 int corpus_word_no) {
	if (remove) WordUtils::UpdateTableFromWord(word, -1);

	int tables = author->getTables();
	vector<double> log_pr(tables + 1, 0.0);

	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();
	assert(topics >= 0);

	int word_id = word->getId();

	// Log_word_pr + log(table number) in each topic.
	// log_pr_topic is log probabilities for sampling topics.
	vector<double> log_pr_topic(topics + 1 , 0.0);

	// f is Log_word_pr for word in each topic.
	vector<double> f(topics+1, log(0));
	double f_new = log(gamma / corpus_word_no);

	log_pr_topic[topics] = f_new;

	int total_tables = 0;
	map<Topic*, int> topic_to_k;

	for (int k = 0; k < topics; k++) {
		Topic* topic = all_topics.getMutableTopic(k);
		topic_to_k[topic] = k;
		f[k] = topic->getLogWordPr(word_id);
		int table_count = topic->getTableCount();
		total_tables += table_count;
		log_pr_topic[k] = f[k] + log(table_count);
		f_new = Utils::LogSum(f_new, 
													f[k] + log(table_count));
	}
	f_new = f_new - log(total_tables + gamma);
	f[topics] = f_new;

	for (int t = 0; t < tables; t++) {
		Table* table = author->getMutableTable(t);
		int table_word_count = table->getWordCount();
		double log_word_count = log(table_word_count);
	
		Topic* topic = table->getMutableTopic();
		int k = topic_to_k[topic];

		log_pr[t] = log_word_count + f[k];
	}

	log_pr[tables] = log(alpha) + f[topics];
	int sample_table = Utils::SampleFromLogPr(log_pr);
	
	
	// Sampled table is a new table or old.
	if (sample_table == tables) {
		// Table is new created.
		author->addNewTable();
		Table* new_table = author->getMutableTable(sample_table);
		word->setTable(new_table);
		
		int sample_topic = Utils::SampleFromLogPr(log_pr_topic);
		if (sample_topic == topics) {
			// Sampled topic is new.
			all_topics.addNewTopic(corpus_word_no);	
		}

		// Set topic for new table.
		Topic* new_topic = all_topics.getMutableTopic(sample_topic);
		new_table->setTopic(new_topic);
		// new_topic->incTableCount(1);
	} else {
		// Table is not new created. 
		// Set table for word.
		Table* new_table = author->getMutableTable(sample_table);
		word->setTable(new_table);
	}

	// Update statistics of table and topic from word
	WordUtils::UpdateTableFromWord(word, 1);
}


double AuthorUtils::AlphaScore(Author* author, 
													 			 double alpha) {
	int tables = author->getTables();
	int words = author->getWordCount();
	double score = 0.0;
	score += tables * log(alpha) +
					 gsl_sf_lngamma(alpha) - 
					 gsl_sf_lngamma(words + alpha);
	for (int i = 0; i < tables; i++) {
		Table* table = author->getMutableTable(i);
		score += gsl_sf_lngamma(table->getWordCount());
	}
	return score;
}

void AuthorUtils::CompactTables(Author* author) {
	int tables = author->getTables();
	for (int i = tables - 1; i >=0 ; --i) {
		if (author->getMutableTable(i)->getWordCount() == 0) {
			author->removeTable(i);	
		}	
	}
}

void AuthorUtils::SampleTopics(Author* author, 
															double gamma, 
															bool remove) {
	int tables = author->getTables();
	
	for (int i = 0; i < tables; i++) {
		Table* table = author->getMutableTable(i);
		TableUtils::SampleTopicForTable(table, gamma, remove);
	}
}


void AuthorUtils::GetTopicsForAuthor(Author* author,
												 				 		 vector<int>& topic_counts,
												 				 		 unordered_map<Topic*, int>& topic_ids) {
	int tables = author->getTables();
	for (int i = 0; i < tables; i++) {
		Table* table = author->getMutableTable(i);
		Topic* topic = table->getMutableTopic();
		auto found = topic_ids.find(topic);
		assert(found != topic_ids.end());
		int topic_id = found->second;
		topic_counts[topic_id] += table->getWordCount();
	}

}
// =======================================================================
// AllAuthors
// =======================================================================

AllAuthors& AllAuthors::GetInstance() {
	static AllAuthors instance;
	return instance;
}

AllAuthors::~AllAuthors() {
	for (auto& author : authors_) {
		delete author;
		author = nullptr;
	}
}

// =======================================================================
// AllAuthorsUtils
// =======================================================================

void AllAuthorsUtils::SaveAuthors(const string& filename_author_counts) {
	ofstream ofs(filename_author_counts);
	AllTopics& all_topics = AllTopics::GetInstance();
	int topics = all_topics.getTopics();
	unordered_map<Topic*, int> m = all_topics.getTopicIds();
	vector<int> topic_counts(topics, 0);
	AllAuthors& all_authors = AllAuthors::GetInstance();
	int authors = all_authors.getAuthors();


	for (int i = 0; i < authors; i++) {
		Author* author = all_authors.getMutableAuthor(i);
		AuthorUtils::GetTopicsForAuthor(author, topic_counts, m);
		
		for (auto& count : topic_counts) {
			ofs << count << " ";
		}
		ofs << endl;

		fill(begin(topic_counts), end(topic_counts), 0);
	}
	ofs.close();
}

} // ahdp