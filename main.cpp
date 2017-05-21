#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> wordList, filteredWordList;

void loadWordList();
void createFilteredWordList();
void filterKnownWords();
void loadFrequentWordList(unsigned int howFrequent, vector<string> *frequentWordList);

string lowercaseString(string *toLowercase);

int main()
{
	cout << ">> Starting project fishWords\n\n\n";
	cout << ">> Loading list of words to memory..." << endl;
	loadWordList();
	filterKnownWords();
//																														DEBUG
//																														{
//	vector<string> test;
//	loadFrequentWordList(10, &test);
//	for(auto i:test)
//		cout << i << endl;
//																														}
//																														/DEBUG
	return 0;
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
	else cerr << "<< subtitle.srt couldn't be read!" << endl;
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

void filterKnownWords()
{
	cout << ">> Removing already known words..." << endl;
	string knownWord;
	vector<string> knownWordList;
	vector<string>::iterator it;
	ifstream knownWords("knownWords.txt");
	if(!knownWords.good())
	{
		cerr << "knownWords.txt couldn't be read!" << endl;
		return;
	}
	while(getline(knownWords, knownWord))
		knownWordList.push_back(knownWord);
	sort(knownWordList.begin(), knownWordList.end());
	vector<string> unknownWordList(knownWordList.size()+filteredWordList.size());
	it=set_difference(filteredWordList.begin(), filteredWordList.end(), knownWordList.begin(), knownWordList.end(), unknownWordList.begin());
	unknownWordList.resize(it-unknownWordList.begin());
	filteredWordList.swap(unknownWordList);
	cout << "<< Obtained a list of " << (filteredWordList.size()) << " unknown words." << endl << endl;
//	for (it=filteredWordList.begin(); it!=filteredWordList.end(); ++it)													//DEBUG
//		std::cout << endl << *it;
//	std::cout << '\n';
}

void loadFrequentWordList(unsigned int howFrequent, vector<string> *frequentWordList)
{
	unsigned int wordsLoaded=0;
	ifstream wordFrequencyList("frequencyList.txt");
	if(!wordFrequencyList.good())
	{
		cerr << "frequencyList.txt couldn't be read!" << endl;
		return;
	}
	string currentWord;
	while(getline(wordFrequencyList, currentWord))																		//load wordListUniqueSorted to a vector
	{
		wordsLoaded++;
		frequentWordList->push_back(currentWord);
		currentWord.clear();
		if(wordsLoaded==howFrequent)
			break;
	}
	wordFrequencyList.close();
}