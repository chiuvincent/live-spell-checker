#include "StudentUndo.h"
#include <cstring>
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	//handling batches
	if (!m_steps.empty()) {
		//consecutive deletes
		if (action == Action::DELETE && m_steps.top().m_act == Action::DELETE) {
			//consecutive deletes at same location
			if (col == m_steps.top().m_col) { 
				m_steps.top().m_str += ch;
				m_steps.top().m_batch++;
				return;
			}
			//consecutive backspaces
			if (col == m_steps.top().m_col - 1) {
				m_steps.top().m_col = col;
				//m_steps.top().m_str += ch;
				string c;
				c += ch;
				m_steps.top().m_str.insert(0, c);
				m_steps.top().m_batch++;
				return;
			}
		}
		//consecutive inserts
		else if (action == Action::INSERT && m_steps.top().m_act == Action::INSERT) {
			if (ch != ' ' && col == m_steps.top().m_col + 1) { 
				//update information
				m_steps.top().m_col = col;
				//m_steps.top().m_str += ch; prolly don't need this
				m_steps.top().m_batch++;
				return;
			}
		}
	}
	//regular submit
	string s;
	s += ch;
	Step sub(action, row, col, s);
	m_steps.push(sub);
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
	if (!m_steps.empty()) {
		Step retrieve = m_steps.top();
		m_steps.pop();

		row = retrieve.m_row;
		col = retrieve.m_col;
		count = 1; //as requested by spec
		text = ""; //as requested by spec

		switch (retrieve.m_act) {
			//Insert -> Delete
		case Action::INSERT:
			count = retrieve.m_batch;
			return Action::DELETE;
			break;
			//Delete -> Insert
		case Action::DELETE:
			text = retrieve.m_str;
			return Action::INSERT;
			break;
			//Join -> Split
		case Action::JOIN:
			return Action::SPLIT;
			break;
			//Split -> Join
		case Action::SPLIT:
			return Action::JOIN;
			break;
		default:
			return Action::ERROR;
		}
	}
	
    return Action::ERROR;  
}

void StudentUndo::clear() {
	while (!m_steps.empty()) {
		m_steps.pop();
	}
}
