#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
#include <string>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct Step {
		Step(Action action, int row, int col, std::string str, int batch=0) 
			: m_act(action), m_row(row), m_col(col), m_str(str), m_batch(batch)  {};
		Action m_act;
		int m_row;
		int m_col;
		std::string m_str; //char or string
		int m_batch; //counts batch length
	};
	std::stack<Step> m_steps;
};

#endif // STUDENTUNDO_H_
