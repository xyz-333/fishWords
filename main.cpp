#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> wordList;

void loadWordList();

int main()
{
	cout << ">> Starting project fishWords\n\n\n";
	cout << ">> Loading list of words to memory..." << endl;
	loadWordList();
	return 0;
}

void loadWordList()
{
	char currentChar='a';
	string currentWord;
	ifstream subtitles;
	subtitles.open("subtitles.srt", ios::in);
	if(subtitles.good())
	{
		while(subtitles.get(currentChar))
		{
			if(isalpha(currentChar)) 																					// TODO: make it work for words with diacritics.
				currentWord+=currentChar;
			else
			{
				if(!currentWord.empty())
				{
					if(currentWord.length()>2&&currentChar!='\'')														// omits 1 and 2 letter words  and those with apostrophes
						wordList.push_back(currentWord);
					currentWord.clear();
				}
			}
		}
		subtitles.close();
		cout << "<< Successfully loaded " << wordList.size() << " words from the subtitles." << endl;
		sort(wordList.begin(), wordList.end());																			// sort words and store only unique ones.
		wordList.erase(unique(wordList.begin(), wordList.end()), wordList.end());
		cout << "<< Obtained a list of " << wordList.size() << " unique words." << endl;
	}
	else cerr << "<< Subtitles couldn't be read!" << endl;
}