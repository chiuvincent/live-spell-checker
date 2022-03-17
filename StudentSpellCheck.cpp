#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

SpellCheck* createSpellCheck()
{
	
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	delete root;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	if (!infile) {
		return false;
	}
	delete root; 
	root = new Trie('\0');
	string entry;
	while (getline(infile, entry)) {
		addWord(root, entry);
	}
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	if (wordExist(root, word)) {
		return true; //return true if word exists
	}
	else {
		suggestions.clear();
		for (int i = 0; i < word.size() && max_suggestions != 0; i++) {
			//look for words in dict that differ by 1 char while max suggestions !=0
			wordSuggestion(root, i, word, max_suggestions, suggestions);
		}
		return false;
	}
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	//set up variables
	problems.clear();
	string iterate = line;
	bool go = true;
	Position pos;
	//go through line
	for (int i = 0; i < iterate.size(); i++) {
		//if we reach a word
		if (go && getIndex(iterate[i]) != -1) {
			pos.start = i; //save start position
			go = false;
		}
		//if we go past a word
		else if (!go && getIndex(iterate[i]) == -1) {
			pos.end = i - 1; //save end position
			problems.push_back(pos);
			go = true;
		}
		//word at the end of line
		else if (!go && i == iterate.size() - 1) {
			pos.end = i; //save end position
			problems.push_back(pos);
			go = true;
		}
	}
	//check words
	vector<Position>::iterator it = problems.begin();
	while (it != problems.end()) {
		int begin = (*it).start;
		int length = 1 + (*it).end - begin;
		string word = iterate.substr(begin, length);
		//remove words that exist
		if (wordExist(root, word)) {
			it = problems.erase(it);
		}
		else {
			it++;
		}
	}
}

int StudentSpellCheck::getIndex(char& c) {
	c = tolower(c);
	if (c >= 97 && c <= 122) { //a-z
		return c - 97;
	}
	else if (c == '\'') {
		return 26; //27th index
	}
	else {
		return -1;
	}
}

void StudentSpellCheck::addWord(Trie* t, string word) {
	//assume tree root's been loaded
	Trie* walker = t;
	for (int i = 0; i < word.size(); i++) {
		int j = getIndex(word[i]);
		if (j == -1) {
			continue; //ignore non a-z + ' chars
		}
		//add new node if such node doesn't exist yet
		if (walker->children[j] == nullptr) {
			walker->children[j] = new Trie(word[i]);
		}
		if (i == word.size() - 1) {
			walker->children[j]->end = true;
		}
		else {
			walker = walker->children[j];
		}
	}
}

bool StudentSpellCheck::wordExist(Trie* t, string word) {
	Trie* walker = t;
	for (int i = 0; i < word.size(); i++) {
		int j = getIndex(word[i]);
		if (j == -1) {
			continue; //ignore this char
		}
		if (walker->children[j] == nullptr) { //word not in tree
			return false;
		}
		else if (walker->children[j]->content == word[i]) {
			if (i == word.size() - 1 && walker->children[j]->end == true) { //reach end of word
				return true;
			}
			else {
				walker = walker->children[j]; //move down tree
			}
		}
	}
	return false;
}

void StudentSpellCheck::wordSuggestion(Trie* t, int index, string word, int remainder, vector<string>& suggestions) {
	if (remainder <= 0) {
		return;
	}
	Trie* walker = t;
	//check if substring before index exist
	string front = word.substr(0, index);
	for (int i = 0; i < front.size(); i++) {
		int j = getIndex(front[i]);
		if (j == -1) {
			continue; //ignore
		}
		if (walker->children[j] == nullptr) {
			return; //no entry exist, quit function
		}
		else if (walker->children[j]->content == front[i]) {
			walker = walker->children[j];
		}
	}

	//substring after index
	string back = word.substr(index + 1, word.size() - (index + 1));

	//check if substring after index exist
	for (int i = 0; i < 27; i++) {
		if (walker->children[i] != nullptr) {
			Trie* walker2 = walker->children[i];
			//if back part matches or last char is index
			if ((wordExist(walker2, back)) || (index == word.size() - 1 && walker2->end)) {
				string suggest = front + walker2->content + back;
				suggestions.push_back(suggest);
				remainder--; 
				if (remainder == 0) {
					return;
				}
			}
		}
	}
	return;
}