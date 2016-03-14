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
} // ahdp
#endif
