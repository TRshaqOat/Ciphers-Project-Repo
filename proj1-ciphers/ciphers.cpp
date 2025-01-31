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
      cout << "*dictionary read successfully*" << endl;
  } else {
      cout << "Error opening dictionary file!" << endl;
      return 1;
  }


  ifstream quadFile("english_quadgrams.txt");
  vector<string> quadgrams;
  vector<int> counts;
  string quadLine;

  if (quadFile.is_open()) {
      while (getline(quadFile, quadLine)) {
          string quad;
          string stringCount;
          for(char c : quadLine) {
            if(isalpha(c)) {
              quad += c;
            } else if(isdigit(c)){
              stringCount += c;
            } else {
              continue;
            }
          }
          quadgrams.push_back(quad);
          counts.push_back(stoi(stringCount));
      }
      quadFile.close();
      cout << "*quadgrams read successfully*" << endl;
  } else {
      cout << "Error opening quadgram file!" << endl;
      return 1;
  }

  QuadgramScorer scorer(quadgrams, counts);



  cout << endl << "Welcome to Ciphers!" << endl;
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
      computeEnglishnessCommand(scorer);
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    } else if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scorer);
    } else if (command == "F" || command == "f") {
      decryptSubstFileCommand(scorer);
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
  for (char c : s) {
    if(isalpha(c)) {
      rString += toupper(c);
    }
  }
  return rString;
}

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student
  vector<string> quadVector;
  double totalScore = 0.0;
  string sending = cleanScoreString(s);
  if(sending.length() >= 4) {
    for(int i = 0; i < sending.length() - 3; i++) {
      totalScore += scorer.getScore(sending.substr(i, 4));
    }
  }
  return totalScore;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student
  string tempInput;
  cout << "Enter Text to Check Englishness: ";
  getline(cin, tempInput);
  cout << endl << "Englishness of Text: " << scoreString(scorer, tempInput) << endl;
}

// Helper function to swap two characters in a vector of cipher keys
void swapLetters(vector<char>& cipher) {
    int i1 = Random::randInt(25);
    int i2 = Random::randInt(25);
    while (i1 == i2) {
        i2 = Random::randInt(25); // Ensure the indices are different
    }
    swap(cipher[i1], cipher[i2]);
}

// Helper function to perform a single hill-climbing run
vector<char> hillClimbOnce(const QuadgramScorer& scorer, const string& ciphertext) {
    vector<char> bestCipher = genRandomSubstCipher(); // Initial random key
    double bestScore = scoreString(scorer, applySubstCipher(bestCipher, ciphertext));
    
    int noImprovementCount = 0;
    
    while (noImprovementCount < 1000) {
        vector<char> newCipher = bestCipher;
        swapLetters(newCipher);  // Swap two letters to create a new candidate key
        
        double newScore = scoreString(scorer, applySubstCipher(newCipher, ciphertext));
        
        if (newScore > bestScore) {
            bestCipher = newCipher;  // Accept the new cipher if it improves the score
            bestScore = newScore;
            noImprovementCount = 0;  // Reset count as we found an improvement
        } else {
            noImprovementCount++;  // No improvement
        }
    }
    
    return bestCipher;
}

// Helper function to run the hill-climbing algorithm multiple times and select the best result
vector<char> hillClimbMultiple(const QuadgramScorer& scorer, const string& ciphertext) {
    vector<char> bestCipher;
    double bestScore = -INFINITY;
    
    for (int i = 0; i < 25; i++) {
        vector<char> cipher = hillClimbOnce(scorer, ciphertext);
        double score = scoreString(scorer, applySubstCipher(cipher, ciphertext));
        
        if (score > bestScore) {
            bestCipher = cipher;
            bestScore = score;
        }
    }
    
    return bestCipher;
}


void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student
  string tempInput;
  cout << "Enter Text to Decrpyt: ";
  getline(cin, tempInput);

  vector<char> bestCipher = hillClimbMultiple(scorer, tempInput);
  string decryptedText = applySubstCipher(bestCipher, tempInput);

  cout << endl << "Decrypted Text: " << decryptedText << endl;
}

void decryptSubstFileCommand(const QuadgramScorer& scorer) {
  // TODO: student
  string fileInput, fileOutput;
  cout << "Enter Input File to Decrpyt: ";
  getline(cin, fileInput);
  cout << "Enter Output File: ";
  getline(cin, fileOutput);

  ifstream file(fileInput);
  string line;
  string finalText;
  while (getline(file, line)) {
    finalText += line + "\n";
  }
  file.close();
  cout << "*Input file read successfully*" << endl;

  vector<char> bestCipher = hillClimbMultiple(scorer, finalText);
  string decryptedText = applySubstCipher(bestCipher, finalText);

  ofstream outputFile(fileOutput);
  outputFile << decryptedText;
  outputFile.close();
}

#pragma endregion SubstDec
