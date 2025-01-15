#pragma once

#include <cassert>
#include <cmath>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

// Don't modify this! It's a hack to let us directly test the main function,
// while obeying the rule that a program can only have one main.
#ifdef COMPILED_FOR_GTEST
#define main ciphers_main
#endif

using namespace std;

// This file is OUT OF SCOPE for you! Feel free to read, but you only have to
// use these as directed in the project guide. You don't need to understand
// or reimplement this file.

class QuadgramScorer {
 private:
  static const size_t N_QUADGRAMS = 26 * 26 * 26 * 26;
  // Log likelihood of each quadgram
  double log_likelihoods[N_QUADGRAMS] = {0};

  // Lightly enforce singleton pattern
  QuadgramScorer(const QuadgramScorer&) = delete;
  QuadgramScorer& operator=(const QuadgramScorer&) = delete;

  static size_t quadgramIndex(const string& quadgram) {
    size_t idx = 0;
    for (const char c : quadgram) {
      idx *= 26;
      idx += (c - 'A');
    }
    return idx;
  }

 public:
  QuadgramScorer(const vector<string>& quadgrams, const vector<int>& counts)
      : log_likelihoods() {
    double total = 0;
    for (int c : counts) {
      total += c;
    }

    // Default to extremely small log likelihood for unknown quadgrams
    double notFoundLikelihood = -log10(total);
    for (size_t i = 0; i < N_QUADGRAMS; i++) {
      log_likelihoods[i] = notFoundLikelihood;
    }

    for (size_t i = 0; i < quadgrams.size(); i++) {
      log_likelihoods[quadgramIndex(quadgrams.at(i))] =
          (double)log10(counts.at(i)) + notFoundLikelihood;
    }
  }

  /**
   * Return the log likelihood of the given quadgram.
   * Quadgram must only contain uppercase letters.
   */
  double getScore(const string& quadgram) const {
    if (quadgram.length() != 4) {
      throw invalid_argument("String passed to getScore, <" + quadgram +
                             ">, is not length 4");
    }
    for (const auto c : quadgram) {
      if (!isupper(c)) {
        throw invalid_argument(
            "Quadgram <" + quadgram +
            "> has character(s) that are not uppercase letters");
      }
    }

    return log_likelihoods[quadgramIndex(quadgram)];
  }
};

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

// Inline the function to, again, prevent ODR issues.
// It's not really the _right_ way to do it, but...

/**
 * Generate a random substitution cipher key
 */
inline vector<char> genRandomSubstCipher() {
  // Fisher-Yates (https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
  vector<char> cipher;
  for (char c = 'A'; c <= 'Z'; c++) {
    cipher.push_back(c);
  }

  for (int i = cipher.size() - 1; i >= 1; i--) {
    size_t j = Random::randInt(i - 1);
    swap(cipher.at(i), cipher.at(j));
  }

  return cipher;
}
