#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list>
#include <string>
#include <algorithm>

class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	int m_row; //cursor row
	int m_col; //cursor col
	std::list<std::string> m_text; //text
	std::list<std::string>::iterator m_line;  //row iterator, points to row in list
	bool saveUndo; //save steps to undo
};

#endif // STUDENTTEXTEDITOR_H_
