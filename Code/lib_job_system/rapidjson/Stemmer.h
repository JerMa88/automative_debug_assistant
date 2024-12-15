#ifndef RAPIDJSON_STEMMER_H_
#define RAPIDJSON_STEMMER_H_

#include <vector>
#include <string>

using namespace std;

namespace Stemmer
{
void stem(string& word);

void trim(string& word);

namespace internal
{
size_t firstNonVowelAfterVowel(const string& word, size_t start);

size_t getStartR1(const string& word);

size_t getStartR2(const string& word, size_t startR1);

void changeY(string& word);

void step0(string& word);

bool step1A(string& word);

void step1B(string& word, size_t startR1);

void step1C(string& word);

void step2(string& word, size_t startR1);

void step3(string& word, size_t startR1, size_t startR2);

void step4(string& word, size_t startR2);

void step5(string& word, size_t startR1, size_t startR2);

inline bool isShort(const string& word);

bool special(string& word);

bool isVowel(char ch);

bool isVowelY(char ch);

bool endsWith(const string& word, const string& str);

bool endsInDouble(const string& word);

bool replaceIfExists(string& word, const string& suffix,
                     const string& replacement, size_t start);

bool isValidLIEnding(char ch);

bool containsVowel(const string& word, size_t start, size_t end);
}
}

#endif