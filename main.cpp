#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

#define WORDS_TO_CONFIRM 9
#define WORDS_KNOWN 8000

using namespace std;

vector<string> wordList, filteredWordList, definitionList;

void loadWordList();
void createFilteredWordList();
void filterKnownWords();
void loadFrequentWordList(unsigned int howFrequent, vector<string> *frequentWordList);
void filterFrequentWords(unsigned int wordFrequency);
//void filterProperNames();
void filterWordsManually();
void findDefinitions();
void loadDefinition(ifstream& dictionary);
void test();

string lowercaseString(string toLowercase);
string lemmaOf(string toLemma, unsigned* wasLemmatized);
fstream& goToLine(fstream& file, unsigned int num);

int main()
{

	cout << ">> Starting project fishWords\n\n\n";
	cout << ">> Loading list of words to memory..." << endl;
	loadWordList();
	filterKnownWords();
	filterFrequentWords(WORDS_KNOWN);
	filterWordsManually();
	findDefinitions();
//																														DEBUG
//																														{
//	test();
//																														}
//																														/DEBUG
	return 0;
}

string lowercaseString(string toLowercase)																				// TODO: address the issue of locale or character encoding
{
	string lowercased=toLowercase;
	for(unsigned int j=0; j<toLowercase.size(); j++)
	{
		if(toLowercase[j]>='A'&&toLowercase[j]<='Z')
			lowercased[j]=(char)tolower(toLowercase[j]);
	}
	return lowercased;
}

fstream& goToLine(fstream& file, unsigned int num)
{
	file.seekg(std::ios::beg);
	for(int i=0; i<num-1; ++i){
		file.ignore(numeric_limits<streamsize>::max(),'\n');
	}
	return file;
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
		currentWord=lowercaseString(i);
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
	cout << "\n\nWrite numbers corresponding to the words that you DO NOT know." << endl <<
	"Enter 'x' to omit if you KNOW all 5 words." << endl <<
	"Enter 'q' to skip manual filtering." << endl <<
	"Please don't mark proper names or non-words." << endl <<
	"Unselected words will be added to a list of user's known words." << endl << endl <<
	"::: 0 out of " << filteredWordList.size() << " :::" << endl;
	vector<unsigned int> capitalizedWordsIndices, temp;                                                                 // separating capitalized words
	vector<string> unknownWords;
	string whichWords;
	bool isCapitalized=false, isLastWord=false;
	unsigned int capitalizedWordIndex=0, end=0, filtered=0;
	for(unsigned int i=0; i<wordList.size(); i++)
	{
		if(wordList[i][0]<'A' || wordList[i][0]>'Z')
			break;
		for(unsigned int j=0; j<filteredWordList.size(); j++)
		{
			if(lowercaseString(wordList[i]).compare(filteredWordList.at(j)) == 0)
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
			for(unsigned int j=0; j<=end; j++)
			{
				if(whichWords.compare("q")==0||whichWords.compare("Q")==0)
				{
					knownWords.close();
					filtered=(unsigned int)unknownWords.size();
					while(i<filteredWordList.size())
					{
						unknownWords.push_back(filteredWordList.at(i));
						i++;
					}
					filteredWordList.swap(unknownWords);
					cout << endl << ">> Manual filtering was skipped." << endl;
					cout << "<< Obtained the final list of " << filteredWordList.size() << " words, " <<
							filteredWordList.size()-filtered << " of which were presumed unknown." << endl << endl;
					return;

				}
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
	cout << endl << ">> Manual filtering is finished, obtained the final list of " <<
			filteredWordList.size() << " unknown words." << endl << endl;
}

void findDefinitions()
{
	cout << ">> Finding definitions for unknown words in dictionary..." << endl;
	ifstream dictionary("dictionary.txt");
//	ofstream debug("debug.txt");
	if(!dictionary.good())
	{
		cerr << "Couldn't read dictionary.txt!" << endl;
		return;
	}
	unsigned int foundDefinitions=0, wasLemmatized;
	string currentWord, soughtWord;
	streampos initialPos=0;
	bool canDefinitionBeFound, searchedForLemma;
	for(unsigned int i=0; i<filteredWordList.size(); i++)
	{
		wasLemmatized=0;
		canDefinitionBeFound=true;
		searchedForLemma=false;
		soughtWord=filteredWordList[i];
		initialPos=dictionary.tellg();
		while(canDefinitionBeFound)
		{
			getline(dictionary, currentWord);
			if(currentWord.size()<=3)
				getline(dictionary, currentWord);
			if(isupper(currentWord[0])&&isupper(currentWord[1])&&isupper(currentWord[2])) 								//if current line is a word
			{
				if(isupper(currentWord[currentWord.size()-2])==0)
					continue;
				currentWord.pop_back();																						//delete end of line character
				currentWord=lowercaseString(currentWord);
//				debug << "soughtWord:" << soughtWord << ", currentWord: " << currentWord << endl;						//DEBUG
				if(currentWord.compare(soughtWord)==0)																		//if it is the sought word
				{
					foundDefinitions++;
					loadDefinition(dictionary);
					break;
				}
				if(currentWord.compare(soughtWord)>0)																		//if it is past sought word
				{
					if(!searchedForLemma)																						//if haven't looked for lemma yet
					{
						dictionary.seekg(initialPos, dictionary.beg);
						soughtWord=lemmaOf(soughtWord, &wasLemmatized);
						if(wasLemmatized==1)																						//if lemmatizing was anything but removing -ING or removing -ED
							searchedForLemma=true;
						if(!wasLemmatized)																							//if can't transform to lemma
						{
							canDefinitionBeFound=false;
							definitionList.push_back("X");
						}
						continue;
					}
					else																										//if haven't found lemma either
					{
						definitionList.push_back("X");
						break;
					}
				}
			}
		}
	}
	dictionary.close();
//	debug.close();
	cout << "<< Obtained definitions for " << foundDefinitions << " out of " << filteredWordList.size() << " words..." << endl;
}

void loadDefinition(ifstream& dictionary)
{
	string currentLine, currentDefinition="";
	bool isFirstLine=true, doSkip=false;
	while(getline(dictionary, currentLine))																				//--> skip to definition
	{
		if(currentLine.length()<=1)
			break;
		currentLine.clear();
	}																													//<--
	while(getline(dictionary, currentLine))
	{
		if(isFirstLine)
		{
			isFirstLine=false;
			if(currentLine[4]==':')
				doSkip=true;
			else if(currentLine[0]=='1')
			{
				if(currentLine[3]=='(')
				{
					getline(dictionary, currentLine);
					getline(dictionary, currentLine);
				}
				doSkip=true;
			}
			else
				break;
		}
		if(currentLine.length()<=1)                                                                               		//if definition has ended
		{
			if(doSkip)
				break;
			else
				isFirstLine=true;
		}
		currentLine.pop_back();																							//delete end of line character
		currentDefinition.append(" ");
		currentDefinition.append(currentLine);
	}
	if(currentDefinition[1]=='1')																						//if definition starts with "1. "
		currentDefinition=(currentDefinition.substr(4, currentDefinition.size()-4));
	else if(currentDefinition[5]==':')																					//if definition starts with 'Defn: "
		currentDefinition=(currentDefinition.substr(7, currentDefinition.size()-7));
	for(unsigned i=0; i<currentDefinition.size(); i++)
	{
		if(currentDefinition[i]=='.')
		{
			if(currentDefinition[i-1]!='p'&&currentDefinition[i-2]!='s')												//delete part of the definition after first dot, but not when there's 'sp' before it, like in "esp. [...]"
			currentDefinition=currentDefinition.substr(0, i);
		}
	}
	definitionList.push_back(currentDefinition);
}

string lemmaOf(string toLemma, unsigned* wasLemmatized)
{
	string lemma=toLemma;
	if(*wasLemmatized==2)																								// if word was already lemmatized by removing "ING" or "ED"
	{
		lemma.push_back('e');
		*wasLemmatized=1;
		return lemma;
	}
	if((lemma.back()=='s'||lemma.back()=='r'||lemma.back()=='d')&&														// if word ends with "IES" or "IER" or "IED".
	   lemma[lemma.size()-2]=='e'&&lemma[lemma.size()-3]=='i')
	{
		lemma.resize(lemma.size()-3);
		lemma+='y';
		*wasLemmatized=1;
		return lemma;
	}
	if(lemma.back()=='s')																								// if word ends with "S".
	{
		lemma.pop_back();
		*wasLemmatized=1;
		return lemma;
	}
	if(lemma.back()=='g'&&lemma[lemma.size()-2]=='n'&&(lemma)[lemma.size()-3]=='i')										// if word ends with "ING".
	{
		lemma.resize(lemma.size()-3);
		*wasLemmatized=2;
		return lemma;
	}
	if(lemma.back()=='t'&&lemma[lemma.size()-2]=='s'&&lemma[lemma.size()-3]=='e')										// if word ends with "EST".
	{
		lemma.resize(lemma.size()-3);
		*wasLemmatized=1;
		return lemma;
	}
	if(lemma.back()=='d'&&lemma[lemma.size()-2]=='e')																	// if word ends with "ED".
	{
		lemma.resize(lemma.size()-2);
		*wasLemmatized=2;
		return lemma;
	}
	*wasLemmatized=0;
	return lemma;
}

void test()
{
	for(int i=0; i<filteredWordList.size(); i++)
	{
		cout << filteredWordList[i] << ":\n" << definitionList[i] << endl << endl;
	}

//	unsigned wasLemmatized=0;
//	cout << lemmaOf(" sharpest", &wasLemmatized) << endl;
}