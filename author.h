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

	Author(const Author& from) = delete;
	Author& operator=(const Author& from) = delete;

	Author(Author&& from) = default;
	Author& operator=(Author&& from);

	int getId() const { return id_; }
	void setId(const int& id) { id_ = id; }

	Word* getMutableWord(int i) { return words_[i]; }
	void addWord(Word* word) { words_.push_back(word); }

	Table* getMutableTable(int i) { return tables_[i]; }
	void addNewTable();
	void removeTable(int pos);

	~Author();
	
private:
	// Id.
	int id_;

	// Words.
	vector<Word*> words_;

	// Tables.
	vector<Table*> tables_;
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