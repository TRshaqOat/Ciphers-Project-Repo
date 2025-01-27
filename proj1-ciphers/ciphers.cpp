#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

// When you add a new helper function, make sure to declare it up here!

/**
 * Print instructions for using the program.
 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;

  ifstream file("dictionary.txt");
  vector<string> dictionary;
  string wordRead;

  if (file.is_open()) {
      while (file >> wordRead) {
          dictionary.push_back(wordRead);
      }
      file.close();
      cout << "*dictionary read successfully*";
  } else {
      cout << "Error opening dictionary file!" << endl;
      return 1;
  }


  ifstream quadFile("english_quadgrams.txt");
  vector<string> quadgrams;
  string quadWord;

  if (quadFile.is_open()) {
      while (quadFile >> quadWord) {
          quadgrams.push_back(quadWord);
      }
      quadFile.close();
      cout << "*quadgrams read successfully*";
  } else {
      cout << "Error opening quadgram file!" << endl;
      return 1;
  }

  QuadgramScorer scorer(inpQuadgrams, counts);



  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    } else if (command == "C" || command == "c") {
      caesarEncryptCommand();
    } else if (command == "D" || command == "d") {
      caesarDecryptCommand(dictionary);
    } else if (command == "E" || command == "e") {

    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    } else if (command == "S" || command == "s") {
      
    } else if (command == "F" || command == "f") {
      
    } 
    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  // TODO: student
  int index = ALPHABET.find(c);
  index = (index + amount) % 26;
  return ALPHABET.at(index);
}

string rot(const string& line, int amount) {
  // TODO: student
  string temp;
  for (char c : line) {
    if(isalpha(c)) {
      char upper = toupper(c);
      temp += rot(upper, amount);
    } else if(isspace(c)) {
      temp += c;
    }
  }
  return temp;
}

void caesarEncryptCommand() {
  // TODO: student
  string normalText;
  string amount;
  cout << "Enter text for encryption: ";
  getline(cin, normalText);
  cout << "Enter an integer for shift encryption: ";
  getline(cin, amount);
  cout << endl << "Encrypted text: " << rot(normalText, stoi(amount)) << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

char rotateDecrypt(char c, int amount) {
  // TODO: student
  int index = ALPHABET.find(c);
  index = (index + amount) % 26;
  return ALPHABET.at(index);
}

void rot(vector<string>& strings, int amount) {
  // TODO: student
  for(int i = 0; i < strings.size(); i++) {
    string temp;
    for (char c : strings.at(i)) {
        temp += rotateDecrypt(c, amount);
    }
    strings.at(i) = temp;
  }
}

string clean(const string& s) {
  // TODO: student
  string rString;
  bool checkChar = false;
  for (char c : s) {
    if(isalpha(c)) {
      rString += toupper(c);
      checkChar = true;
    } else if(isspace(c) && checkChar && s[s.length() - 2] != c) {
      rString += c;
    }
  }
  return rString;
}

vector<string> splitBySpaces(const string& s) {
  // TODO: student
  vector<std::string> result;
  stringstream ss(s);
  string word;
  while (ss >> word) {
      result.push_back(word);
  }
  return result;
}

string joinWithSpaces(const vector<string>& words) {
  // TODO: student
  string jointPhrase;
  for(int i = 0; i < words.size(); i++) {
    for (char c : words.at(i)) {
      jointPhrase += c;
    }
    if(i != words.size() - 1) {
      jointPhrase += ' ';
    }
  }
  return jointPhrase;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  // TODO: student
  int count = 0;
  for(int i = 0; i < words.size(); i++) {
    bool check = false;
    for(int j = 0; j < dict.size(); j++) {
      if(dict.at(j) == words.at(i)) {
        count++;
        break;
      }
    }
  }
  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  // TODO: student
  string tempInput;
  cout << "Enter Text For Decryption: ";
  getline(cin, tempInput);
  string cleanedInput = clean(tempInput);
  vector<string> words = splitBySpaces(cleanedInput);
  bool checkGood = false;
  for(int i = 0; i <= 26; i++) {
    vector<string> newWords = words;
    rot(newWords, i);
    if(numWordsIn(newWords, dict) > newWords.size() / 2) {
      cout << joinWithSpaces(newWords) << endl;
      checkGood = true;
    }
  }
  if(!checkGood) {
    cout << endl << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  // TODO: student
  string encryptedString;
  for (char c : s) {
    if(isalpha(c)) {
      char upper = toupper(c);
      int index = 0;
      for(int i = 0; i < ALPHABET.size(); i++) {
        if(upper == ALPHABET.at(i)) {
          index = i;
          break;
        }
      }
      encryptedString += cipher.at(index);
    } else {
      encryptedString += c;
    }
  }
  return encryptedString;
}

void applyRandSubstCipherCommand() {
  // TODO: student
  string tempInput;
  cout << "Enter Text For Encryption: ";
  getline(cin, tempInput);
  vector<char> cipher = genRandomSubstCipher(); 
  cout << endl << "Encrypted text: " << applySubstCipher(cipher, tempInput) << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

string cleanScoreString(const string& s) {
  // TODO: student
  string rString;
  bool checkChar = false;
  for (char c : s) {
    if(isalpha(c)) {
      rString += toupper(c);
      checkChar = true;
    } else if(isspace(c) && checkChar && s[s.length() - 2] != c) {
      rString += c;
    }
  }
  return rString;
}

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student
  vector<string> quadVector;
  string sending = cleanScoreString(s);
  if(s.length() >= 4) {
    for(int i = 0; i < sending.length() - 3; i++) {
      quadVector.push_back(sending.substr(i, 4));
    }
  }
  return scorer.getScore(quadVector);
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student
    cout << endl << "Englishness of scorer: " << applySubstCipher(cipher, tempInput) << endl;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer, const string& ciphertext) {
  // TODO: student
  return vector<char>{};
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student
}

#pragma endregion SubstDec
