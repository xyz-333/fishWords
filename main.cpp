#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#define WORDS_TO_CONFIRM 9

using namespace std;

vector<string> wordList, filteredWordList;
long int pos;

void loadWordList();
void createFilteredWordList();
void filterKnownWords();
void loadFrequentWordList(unsigned int howFrequent, vector<string> *frequentWordList);
void filterFrequentWords(unsigned int wordFrequency);
//void filterProperNames();
void filterWordsManually();

string lowercaseString(string *toLowercase);

int main()
{
	cout << ">> Starting project fishWords\n\n\n";
	cout << ">> Loading list of words to memory..." << endl;
	loadWordList();
	filterKnownWords();
	filterFrequentWords(8000);
	filterWordsManually();
//																														DEBUG
//																														{
//	vector<string> test;
//	loadFrequentWordList(10, &test);
	for(auto i:filteredWordList)
		cout << i << endl;
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
	cout << "\t>> Loading frequent words to memory..." << endl;
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
	pos=wordFrequencyList.tellg();
	wordFrequencyList.close();
	cout << "\t<< Successfuly loaded top " << howFrequent << " most frequent words." << endl;
}

void filterFrequentWords(unsigned int wordFrequency)
{
	cout << ">> Removing presumably known words..." << endl;
	bool isWordKnown;
	vector<string> frequentWordList, tempWordList;
	loadFrequentWordList(wordFrequency, &frequentWordList);
	for(auto i:filteredWordList)
	{
		isWordKnown=false;
		for(auto j:frequentWordList)
		{
			if(i.compare(j)==0)
			{
				isWordKnown=true;
				break;
			}
		}
		if(!isWordKnown)
			tempWordList.push_back(i);
	}
	filteredWordList.swap(tempWordList);
	cout << "<< Obtained a list of " << filteredWordList.size() << " rare words." << endl << endl;
}

void filterWordsManually()																								//TODO: split to smaller functions
{
	char proceed;
	cout << ">> Proceeding to manual filtering.\n\t>> Enter \'y\' if you wish to continue or \'n\' to skip... \n\t<< ";
	cin >> proceed;
	if(proceed=='n'||proceed=='N')
	{
		cout << endl << ">> Manual filtering was canceled, all words are presumed unknown." << endl << endl;
		return;
	}
	ofstream knownWords("knownWords.txt", std::ios::app);
	if(!knownWords.good())
	{
		cerr << "knownWords.txt couldn't be written!" << endl;
		return;
	}
	string whichWords;
	cout << "\n\nWrite numbers corresponding to the words that you DO NOT know." << endl <<
	"Write x to skip if you KNOW all 5 words." << endl <<
	"Please don't mark proper names or non-words." << endl <<
	"Unselected words will be added to a list of user's known words." << endl << endl <<
	"::: 0 out of " << filteredWordList.size() << " :::" << endl;
	vector<unsigned int> capitalizedWordsIndices, temp;                                                                 // separating capitalized words
	vector<string> unknownWords;
	bool isCapitalized=false, isLastWord=false;
	unsigned int capitalizedWordIndex=0, end=0;
	for(unsigned int i=0; i<wordList.size(); i++)
	{
		if(wordList[i][0]<'A' || wordList[i][0]>'Z')
			break;
		for(unsigned int j=0; j<filteredWordList.size(); j++)
		{
			if(lowercaseString(&wordList[i]).compare(filteredWordList.at(j)) == 0)
			{
				capitalizedWordsIndices.push_back(i);
				temp.push_back(j);
			}
		}
	}
	for(unsigned int i=0; i<filteredWordList.size(); i++)
	{
		cout << (i%WORDS_TO_CONFIRM)+1 << ". ";
		for(unsigned int k=0; k<temp.size(); k++)
		{
			isCapitalized=false;
			if(i == temp[k])
			{
				isCapitalized=true;
				capitalizedWordIndex=capitalizedWordsIndices[k];
				break;
			}
		}
		if(isCapitalized)
			cout << wordList.at(capitalizedWordIndex);
		else
			cout << filteredWordList.at(i);
		cout << endl;
		end=WORDS_TO_CONFIRM-1;
		if(i==filteredWordList.size()-1)
		{
			cout << i << endl;
			isLastWord=true;
			end=i%WORDS_TO_CONFIRM;
		}
		if((i%WORDS_TO_CONFIRM)+1==WORDS_TO_CONFIRM||isLastWord)
		{
			cout << endl << ">> ";
			cin >> whichWords;
			for(int j=0; j<=end; j++)
			{
				if(whichWords.find(to_string(j+1))==string::npos)														//if user didn't mark given word as unknown.
				{
					knownWords << filteredWordList.at(i-end+j) << endl;
				}
				else
				{
					unknownWords.push_back(filteredWordList.at(i-end+j));
				}
			}
			cout << endl << "::: " << i+1 << " out of " << filteredWordList.size() << " :::" << endl << endl;
		}
	}
	filteredWordList.swap(unknownWords);
	knownWords.close();
	cout << endl << ">> Manual filtering is finished, obtained the final list of " << filteredWordList.size() << " unknown words." << endl << endl;
}

//	ofstream chosenWords("chosenWordList.txt");
//	ofstream userUnknownWords;
//	userUnknownWords.open("userUnknownWords.txt", ios_base::app);
//	for(auto i:wordList)
//	{
//		if(!(i.empty()))
//		{
//			chosenWords << i << endl;
//			userUnknownWords << i << endl;
//		}
//	}
//	cout << "Successfully obtained a list of " << loadedWordsCount-knownWordsCount << " words by manually removing " <<
//	knownWordsCount << " known words..." << endl << endl;
//	filteredWords.close();
//	chosenWords.close();
//	userUnknownWords.close();
//	knownWordsList.close();
//}

//void filterProperNames()
//{
//	ifstream wordFrequencyList("frequencyList.txt");
//	if(!wordFrequencyList.good())
//	{
//		cerr << "filteredWordList or wordFrequencyList couldn't be read!" << endl;
//		return;
//	}
//	vector<string> presumedProperNames, properNames;
//	string currentWord, presumedProperName;
//	bool isProperName;
//	for(auto i:wordList)
//	{
//		if(i[0]<'A'||i[0]>'Z')
//			break;
//		presumedProperNames.push_back(lowercaseString(&i));
//	}
//	wordFrequencyList.seekg(pos, wordFrequencyList.beg);
//	while(getline(wordFrequencyList, currentWord))
//	{
//		for(auto i:presumedProperNames)
//		{
//			isProperName=true;
//			if(i.compare(currentWord)==0)
//			{
//				isProperName=false;
//				break;
//			}
//			if(isProperName)
//
//
////			if(isProperName)																							// DEBUG: display presumed proper names.
////				cout << *i << endl;
//		}
//
//		currentWord.clear();
//	}