#ifndef STATE_H_
#define STATE_H_

#include <map>

#include "document.h"
#include "topic.h"
#include "corpus.h"


namespace ahdp {

class StateUtils {
public:
	static bool CheckSumForTopic(Topic* topic);

	static bool CheckSumForAllTopics();

	static bool CheckSumForAuthor(Author* author);

	static bool CheckSumForAllAuthors();

	static bool CheckSumForTable(Table* table);

	static bool CheckSum();
private: 

	static void UpdateTopicWordCounts(Topic* topic, int update);
	static void UpdateTopicTableCounts(Topic* topic, int update);

	static map<Topic*, int> topic_word_counts_;
	static map<Topic*, int> topic_table_counts_;
	

};


} // namespace ahdp


#endif // STATE_H_
