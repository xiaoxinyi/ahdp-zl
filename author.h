#ifndef AUTHOR_H_
#define AUTHOR_H_

#include <vector>

using namespace std;

namespace ahdp {

class Word;
class Table;

// Author contains an author id, all the  words belongs 
// to this author, tables.
class Author {
public:
	Author(int id);	
	~Author();

	Author(const Author& from) = delete;
	Author& operator=(const Author& from) = delete;

	Author(Author&& from) = default;
	Author& operator=(Author&& from);

	int getId() const { return id_; }
	void setId(const int& id) { id_ = id; }

	Word* getMutableWord(int i) { return words_[i]; }
	void addWord(Word* word) { words_.push_back(word); }
	void removeWord(Word* word);

	Table* getMutableTable(int i) { return tables_[i]; }
	void addNewTable();
	void removeTable(int pos);
	int getTables() const { return tables_.size(); }

	int getWordCount() const { return word_count_; }
	void setWordCount(const int& word_count) { word_count_ = word_count; }
	void updateWordCount(int update) { word_count_ += update; }

	
private:
	// Id.
	int id_;

	// Word Count.
	int word_count_;

	// Words.
	vector<Word*> words_;

	// Tables.
	vector<Table*> tables_;
};

// This class provides functionality for sampling tables for
// the words in the author.
class AuthorUtils {
public:
	static void SampleTableForWord(Author* author,
																 Word* word,
																 bool remove,
																 double alpha,
																 double gamma,
																 int corpus_word_no);
	// Sampling tables for the words in the author.
	// permute_words - flag for permuting words.
	// remove - 
	// alpha - second level DP parameter.
	// gamma - first level DP parameer.
	static void SampleTables(Author* author,
													 bool remove,
													 double alpha,
								    			 double gamma,
								    			 int corpus_word_no);

	// Compress the unused tables.
	static void CompactTables(Author* author);

	// Compute alpha score - author partition score.
	static double AlphaScore(Author* author, 
													 double alpha);

	// Sample Topics for all tables given a document.
	static void SampleTopics(Author* author, 
													 double gamma, 
													 bool remove); 

};

// AllAuthors contains all the authors in the corpus.
class AllAuthors {
public:
// Sigleton instance of AllAuthors.
static AllAuthors& GetInstance();

public:
AllAuthors(const AllAuthors& from) = delete;
AllAuthors& operator=(const AllAuthors& from) = delete; 

int getAuthors() const { return authors_.size(); }
void setAuthors(vector<Author>& authors) {
	authors_ = move(authors);
}

Author* getMutableAuthor(int author_id) { return &authors_[author_id]; }
private:
	vector<Author> authors_;
	AllAuthors() {}
	
};

}  // ahdp
#endif