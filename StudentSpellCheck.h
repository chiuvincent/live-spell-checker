#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck() { root = nullptr; }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Trie {
		Trie(char c = 0) {
			content = c;
			end = false;
			for (int i = 0; i < 27; i++) {
				children.push_back(nullptr); //0-25: a-z, 26: '
			}
		}
		~Trie(){
			for (int i = 0; i < 27; i++) {
				delete children[i]; 
			}
		}
		char content;
		bool end;
		std::vector<Trie*> children;
	};

	Trie* root; //dictionary trie
	
	int getIndex(char& c); //return 0-26 for a-z + ' else return -1 
	void addWord(Trie* t, std::string word); 
	bool wordExist(Trie* t, std::string word); //check if word exist in trie
	//look for no more than "remainder" words that differ by 1 char at "index" 
	void wordSuggestion(Trie* t, int index, std::string word, int remainder, std::vector<std::string>& suggestions); 
};



#endif  // STUDENTSPELLCHECK_H_
