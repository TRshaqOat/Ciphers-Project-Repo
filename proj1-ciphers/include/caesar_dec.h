#pragma once

#include <string>
#include <vector>

using namespace std;

// ========== Caesar Cipher Decoder ==========

/**
 * Rotate the letters in every string of `strings` by `amount` characters
 * forward, converting them to uppercase. Note that this function returns void,
 * not a vector. Instead, it should change the contents of the existing vector.
 *
 * You may find it useful to call your rot(string) function from the previous
 * section.
 *
 * For example:
 *  `rot({"A", "hello"}, 1)` modifies the vector to hold `{"B", "IFMMP"}`
 */
void rot(vector<string>& strings, int amount);

/**
 * Given a string `s`, removes all characters that are not letters
 * from the string and uppercases all letters.
 *
 * For example:
 *  `clean("a-b c")` returns `"ABC"`
 */
string clean(const string& s);

/**
 * Given a string `s`, returns a vector where each item is a single word from
 * `s`. There may be multiple spaces between words, spaces at the beginning,
 * or spaces at the end.
 *
 * For example:
 *  `splitBySpaces(" THE   FAT CAT ")` returns `{"THE", "FAT", "CAT"}`
 */
vector<string> splitBySpaces(const string& s);

/**
 * Given a vector `words`, returns a new string with all the elements of `words`
 * concatenated with a single space between each one.
 *
 * For example:
 *  `joinWithSpaces({"THE", "FAT", "CAT"})` returns `"THE FAT CAT"`
 */
string joinWithSpaces(const vector<string>& words);

/**
 * Returns the number of strings in `words` that are also in `dict`.
 *
 * For example:
 *   `howManyWordsIn(
 *     {"THE", "CAT", "IN", "THE", "ZZYZX"},
 *     {"CAT", "COOKIE", "IN", "THE"}
 *    )` returns 4. The words "the", "cat", "in", and "the" are in the given
 *    dictionary.
 */
int numWordsIn(const vector<string>& words, const vector<string>& dict);

/**
 * Runs the Caesar decryption routine. Prompts from the console input
 * (cin) once to get the text to decrypt. For each decryption with more than
 * half its words in the dictionary, prints to the console.
 *
 * You should call the other functions you implemented in this
 * part: `rot`, `clean`, `splitBySpaces`, `join`, and `numWordsIn`.
 */
void caesarDecryptCommand(const vector<string>& dict);
