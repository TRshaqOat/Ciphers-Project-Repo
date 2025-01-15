#pragma once

#include <string>
#include <vector>

#include "utils.h"

using namespace std;

// ========== Englishness Scoring ==========

/**
 * Returns the "score" for the given string `s`. The score for each individual
 * quadgram is calculated by the provided `scorer` object. The total score is
 * the sum of the quadgrams' scores.
 *
 * Assumes that `s` has only uppercase letters, and is at least 4 letters long.
 */
double scoreString(const QuadgramScorer& scorer, const string& s);

/**
 * Runs the command to score the "Englishness" of input text. Prompts from the
 * console input (cin) once to get text. Outputs the Englishness score as
 * determined by `scorer`.
 */
void computeEnglishnessCommand(const QuadgramScorer& scorer);

// ========== Substitution Cipher Decoder ==========

/**
 * Runs the substitution cipher decryption routine. Prompts from the console
 * input (cin) once to get the ciphertext, then runs hill-climbing 25 times to
 * find the best decryption. Outputs the best decryption according to `scorer`.
 *
 * Non-letter characters should be preserved.
 */
void decryptSubstCipherCommand(const QuadgramScorer& scorer);
