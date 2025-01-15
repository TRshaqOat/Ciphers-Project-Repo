#pragma once

#include <string>
#include <vector>

using namespace std;

// ========== Substitution Cipher Encoder ==========

/**
 * Applies the substitution cipher represented by `cipher` to `s`. That is, each
 * letter in `s` should be replaced with whichever letter it maps to in
 * `cipher`.
 *
 * The input may have non-letter characters, which should be left alone.
 */
string applySubstCipher(const vector<char>& cipher, const string& s);

/**
 * Runs the random substitution cipher encryption routine. Prompts from the
 * console input (cin) once to get the text to encrypt. Outputs the text after
 * encryption with a random substitution cipher.
 *
 * Non-letter characters should be preserved.
 */
void applyRandSubstCipherCommand();
