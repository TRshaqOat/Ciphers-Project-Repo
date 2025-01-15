#pragma once

#include <string>
#include <vector>

using namespace std;

// ========== Caesar Cipher Encoder ==========

/**
 * Returns `c` rotated by `amount` many characters forward. If we run out of
 * letters in the alphabet, wrap around back to 'A'. This method assumes that
 * `c` is an uppercase letter.
 *
 * For example:
 * - `rot('A', 0)` returns 'A'
 * - `rot('A', 1)` returns 'B'
 * - `rot('Z', 1)` returns 'A'
 * - `rot('A', 10)` returns 'K'
 * - `rot('J', 25)` returns 'I'
 */
char rot(char c, int amount);

/**
 * Returns a new string in which every character has been rotated by `amount`
 * letters. Lowercase letters are converted to uppercase and rotated.
 * Non-alphabetic characters, except for spaces, are removed.
 *
 * For example:
 * - `rot("A", 0)` returns "A"
 * - `rot("AA", 3)` returns "DD"
 * - `rot("HELLO", 0)` returns "HELLO"
 * - `rot("HELLO", 1)` returns "IFMMP"
 * - `rot("drag on!", 1)` returns "ESBH PO"
 */
string rot(const string& line, int amount);

/**
 * Runs the Caesar cipher encryption routine. Prompts from the console input
 * (cin) twice: once to get the text to encrypt, and once to get the number to
 * rotate by. Prints the result of the caesar shift to the console (cout).
 */
void caesarEncryptCommand();
