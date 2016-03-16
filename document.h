#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <vector>
#include <unordered_map>

using namespace std;

namespace ahdp {

class Table;

// Word has an id, an author id, a table assignment.
class Word {
public:
	Word(int id);

	int getId() const { return id_; }
	void setId(const int& id) { id_ = id; }

	int getAuthorId() const { return author_id_; }
	void setAuthorId(const int& author_id) {
		author_id_ = author_id;
	}

	Table* getMutableTable() { return table_; }
	void setTable(Table* table) { table_ = table; }

private:
	// Word id.
	int id_;

	// author_id;
	int author_id_;

	// A pointer to table which the word is assigned to.
	Table* table_;
	
};

class WordUtils {
public:
	// Update words in author.
	static void UpdateAuthorFromWord(Word* word, int update);

	static void UpdateTableFromWord(Word* word, int update);
};

class Author;

class Document {
public:
	Document(int id);
	~Document();

	Document(const Document& from) = delete;
	Document& operator=(const Document& from) = delete;

	Document(Document&& from) = default;
	Document& operator=(Document&& from);

	int getWords() { return words_.size(); }

	void addWord(int id) { words_.emplace_back(new Word(id)); }
	void addWord(Word* word) { words_.push_back(word); }
	Word* getMutableWord(int i) { return words_[i]; }
	void setWords(vector<Word*>&& words) {
		words_ = move(words);
	}

	int getAuthors() const { return authors_.size(); }
	vector<int>& getAuthorIds() { return authors_; }
	int getAuthor(int i) const { return authors_[i]; }
	vector<Author*> getMutableAuthors() const;

private:
	// Id.
	int id_;

	// Words.
	vector<Word*> words_;

	// Authors.
	vector<int> authors_;
};

// The class provide functionality for 
// sampling author for each word.
class DocumentUtils {
public:
	// Sample author for each word.
	// Dirichlet process parameter,
	static void SampleAuthors(Document* document,
													 int permute_words,
											     bool remove);

	// Permute words in the document.
	static void PermuteWords(Document* document);

	static void SampleAuthorForWord(Document* document, Word* word, bool remove);
};


} // ahdp
#endif
