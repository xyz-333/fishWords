#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> wordList, filteredWordList;

void loadWordList();
void createFilteredWordList();

string lowercaseString(string *toLowercase);

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
			if(isalpha(currentChar)) 																					// TODO: address the issue of locale or character encoding
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
		cout << "<< Successfully loaded " << wordList.size() << " words from the subtitles." << endl << endl;
		cout << ">> Removing duplicates and sorting..." << endl;
		sort(wordList.begin(), wordList.end());																			// sort words and store only unique ones.
		wordList.erase(unique(wordList.begin(), wordList.end()), wordList.end());
		createFilteredWordList();
		cout << "<< Obtained a list of " << filteredWordList.size() << " unique, sorted words." << endl << endl;
	}
	else cerr << "<< Subtitles couldn't be read!" << endl;
}

string lowercaseString(string *toLowercase)																				// TODO: address the issue of locale or character encoding
{
	string lowercased=*toLowercase;
	for(unsigned int j=0; j<(*toLowercase).size(); j++)
	{
		if((*toLowercase)[j]>='A'&&((*toLowercase)[j]<='Z'))
			lowercased[j]=(char)tolower((*toLowercase)[j]);
	}
	return lowercased;
}

void createFilteredWordList()
{
	string currentWord;
	for(auto i:wordList)
	{
		currentWord=lowercaseString(&i);
		filteredWordList.push_back(currentWord);
	}
	sort(filteredWordList.begin(), filteredWordList.end());
	filteredWordList.erase(unique(filteredWordList.begin(), filteredWordList.end()), filteredWordList.end());
}