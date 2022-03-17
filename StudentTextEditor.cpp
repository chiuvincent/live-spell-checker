#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std; 

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	m_row = 0;
	m_col = 0;
	m_text.push_back(""); //initialize empty line
	m_line = m_text.begin();
	saveUndo = true; 
}

StudentTextEditor::~StudentTextEditor()
{
	//nothing dynamically allocated
}

bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile) {
		return false;
	}
	reset();
	string line;
	while (getline(infile, line)) {
		m_text.push_back(line);
	}
	/*char ch;
	while (infile.get(ch)) {
		if (ch == '\r' || ch == '\n') {
			enter();
		}
		else {
			insert(ch);
		}
	}*/
	//reset cursor
	m_row = 0;
	m_col = 0;
	m_line = m_text.begin();
	
	return true;  
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile) {
		return false;
	}
	for (list<string>::iterator it = m_text.begin(); it != m_text.end(); it++) {
		outfile << *it << endl;
	}
	return true; 
}

void StudentTextEditor::reset() {
	getUndo()->clear();
	m_row = 0;
	m_col = 0;
	m_text.clear();
	m_line = m_text.begin();
}

void StudentTextEditor::move(Dir dir) {
	switch (dir) {
	case Dir::UP :
		if (m_row != 0) {
			m_line--;
			m_row--;
			if (m_col > m_line->size()) { //move to last spot in line above
				m_col = m_line->size();
			}
		}
		break;
	case Dir::DOWN:
		if (m_row != m_text.size() - 1) {
			m_line++;
			m_row++;
			if (m_col > m_line->size()) { //move to last spot in line below
				m_col = m_line->size();
			}
		}
		break;
	case Dir::LEFT:
		if (m_col == 0) {
			if (m_row != 0) { //move to last spot in prev line
				m_line--;
				m_row--;
				m_col = m_line->size();
			}
		}
		else {
			m_col--;
		}
		break;
	case Dir::RIGHT:
		if (m_col == m_line->size()) {
			if (m_row != m_text.size() - 1) { //move to first spot in next line
				m_line++;
				m_row++;
				m_col = 0;
			}
		}
		else {
			m_col++;
		}
		break;
	case Dir::HOME:
		m_col = 0;
		break;
	case Dir::END:
		m_col = m_line->size();
		break;
	}
}

void StudentTextEditor::del() {
	string old_line = *m_line;
	string new_line = "";

	//merge lines when deleting
	if (m_col == m_line->size()) {
		if (m_row != m_text.size() - 1) {
			//get next line
			list<string>::iterator next = m_line;
			next++;
			//merge and erase
		    new_line = old_line + *next;
			m_text.erase(next); 
			//replace old line with new line
			m_line = m_text.erase(m_line);
			m_text.insert(m_line, new_line);
			m_line--;
			//save to Undo 
			if (saveUndo) {
				getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
			}
		}
	}

	//delete character 
	else { 
		char deleted = '\0'; //the char to be deleted
		for (int i = 0; i < old_line.size(); i++) {
			if (i != m_col) {
				new_line += old_line.at(i);
			}
			else {
				deleted = old_line.at(i); 
			}
		}
		//replace old line with new line
		m_line = m_text.erase(m_line);
		m_text.insert(m_line, new_line);
		m_line--;
		//save to Undo
		if (saveUndo&& deleted != '\0') {
			getUndo()->submit(Undo::Action::DELETE, m_row, m_col, deleted); 
		}
	}
}

void StudentTextEditor::backspace() {
	string old_line = *m_line;
	string new_line = "";

	//backspace char
	if (m_col > 0) { 
		char deleted = '\0';
		for (int i = 0; i < old_line.size(); i++) {
			if (i != m_col - 1) {
				new_line += old_line.at(i);
			}
			else {
				deleted = old_line.at(i);
			}
		}
		//replace old line with new line
		m_line = m_text.erase(m_line);
		m_text.insert(m_line, new_line);
		m_line--;
		//cursor moves left by 1
		m_col--;
		//save to Undo
		if (saveUndo && deleted != '\0') {
			getUndo()->submit(Undo::Action::DELETE, m_row, m_col, deleted); 
		}
	}

	//merge line backspace
	else {
		if (m_row != 0) {
			//get previous line
			list<string>::iterator prev = m_line;
			prev--;
			//point cursor to previous end
			m_col = prev->size(); 
			m_row--;
			//merge and erase
			new_line = *prev + old_line;
			m_text.erase(prev);
			//replace old line with new line
			m_line = m_text.erase(m_line);
			m_text.insert(m_line, new_line);
			m_line--;
			//save to Undo 
			if (saveUndo) {
				getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
			}
		}
	}
}

void StudentTextEditor::insert(char ch) {
	string old_line = *m_line;
	string new_line = "";
	
	//add character to line
	bool add = false; //no char added yet
	if (m_col == 0) { //insert before
		add = true; //mark as added
		if (ch == '\t') { 
			new_line += "    ";
			m_col += 4;
		}
		else {
			new_line += ch;
			m_col++;
		}
	}

	//add rest of the string
	for (int i = 0; i < old_line.size(); i++) {
		new_line += old_line.at(i);
		if (i == m_col - 1 && !add) {
			add = true; //mark as added
			if (ch == '\t') {
				new_line += "    ";
				m_col += 4;
			}
			else {
				new_line += ch;
				m_col++;
			}
		}
	}
	
	//replace old line with new line
	m_line = m_text.erase(m_line);
	m_text.insert(m_line, new_line);
	m_line--; //move line back to where we're at

	//save to Undo
	if (saveUndo) {
		getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
	}
}

void StudentTextEditor::enter() {
	//save to undo
	if (saveUndo) {
		getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
	}

	//make 2 new lines
	string upper = "";
	string lower = "";
	for (int i = 0; i < m_line->size(); i++) {
		if (i < m_col) {
			upper += m_line->at(i);
		}
		else {
			lower += m_line->at(i);
		}
	}

	//replace old line with 2 new lines
	m_line = m_text.erase(m_line);
	m_text.insert(m_line, upper);
	m_text.insert(m_line, lower);
	m_line--;
	
	//fix cursor
	m_row++;
	m_col = 0;
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_row;
	col = m_col;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if ((startRow < 0 || numRows < 0) || (startRow > m_text.size())) {
		return -1;
	}
	
	lines.clear();
	//walk iterator to startRow
	list<string>::const_iterator it = m_text.begin();
	for (int i = 0; i != startRow; i++) {
		it++;
	}

	//add stuff into lines
	for (int i = 0; i < numRows && i < m_text.size(); i++) {
		lines.push_back(*it);
		it++;
	}
	return 0;
}

void StudentTextEditor::undo() {
	saveUndo = false; //don't save undo moves
	
	//set up variables
	int u_row;
	int u_col;
	int batch_count;
	string word;
	Undo::Action act = getUndo()->get(u_row, u_col, batch_count, word);

	//Undo cases
	if (act != Undo::Action::ERROR) {
		//set variables
		m_row = u_row;
		m_col = u_col;
		m_line = m_text.begin(); //walk line iterator to right spot
		for (int i = 0; i != m_row; i++) {
			m_line++;
		}

		if (act == Undo::Action::DELETE) {
			for (int i = 0; i <= batch_count; i++) {
				backspace();
			}
		}
		else if (act == Undo::Action::INSERT) {
			for (int i = 0; i < word.size(); i++) {
				insert(word.at(i));
			}
		}
		else if (act == Undo::Action::SPLIT) {
			enter();
		}
		else if (act == Undo::Action::JOIN) {
			del();
		}
	}

	saveUndo = true; //turn save back on
}
