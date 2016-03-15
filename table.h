#ifndef TABLE_
#define TABLE_ 

#include <vector>
#include <unordered_map>

using namespace std;

namespace ahdp {

class Topic;

// Table contains number of words assigned to the tablem
// word id and its count, and the topic which the table is
// belonged to.
class Table {
public:
	Table();
	Table(int word_count);

	int getWordCount() const { return word_count_; }
	void setWordCount(const int& word_count) { word_count_= word_count; }
	void updateWordCount(int val) { word_count_ += val; }

	Topic* getMutableTopic() { return topic_; }
	void setTopic(Topic* topic) { topic_ = topic; }

	int getWordCount(int word_id);
	void updateWordCount(int word_id, int update);

	unordered_map<int, int>& getWordCounts() { return word_counts_; }

private:
	// Number of words that assigned to the table.
	int word_count_;

	// Word ids and its counts;
	unordered_map<int, int> word_counts_;

	// The topic which the table is assigned to.
	Topic* topic_;
	
};

// TableUtils provides functionality for update the 
// corresponding topic statistics from the table.
// Remove or add word count
class TableUtils {

// Update topic's statistics from a single word from table.
static void UpdateTopicFromTable(Table* table,
																	int word_id,
																	int update);

// Update topic's statistics from all the words from table.
static void UpdateTopicFromTable(Table* table,
																	int update);
};
} // ahdp

#endif