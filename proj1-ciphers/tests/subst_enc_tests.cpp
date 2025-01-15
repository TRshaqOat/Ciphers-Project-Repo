#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "include/subst_enc.h"
#include "tests/test_utils.h"

using ::testing::HasSubstr;
using ::testing::StrEq;

namespace {

const string FOX_PANGRAM = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
const string MR_PANGRAM_PUNCT = "Mr. Jock, TV quiz PhD, bags few lynx.";

TEST(SubstEnc_ApplyCipher, Identity) {
  vector<char> identity_cipher = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                                  'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                                  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

  ASSERT_THAT(applySubstCipher(identity_cipher, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
              StrEq("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  ASSERT_THAT(applySubstCipher(identity_cipher, FOX_PANGRAM),
              StrEq(FOX_PANGRAM));
  ASSERT_THAT(applySubstCipher(identity_cipher, MR_PANGRAM_PUNCT),
              StrEq("MR. JOCK, TV QUIZ PHD, BAGS FEW LYNX."));
}

TEST(SubstEnc_ApplyCipher, Cipher) {
  vector<char> cipher = {'V', 'Y', 'B', 'L', 'Z', 'O', 'F', 'M', 'A',
                         'I', 'D', 'Q', 'G', 'J', 'K', 'X', 'H', 'N',
                         'W', 'E', 'R', 'S', 'U', 'P', 'C', 'T'};

  ASSERT_THAT(applySubstCipher(cipher, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
              StrEq("VYBLZOFMAIDQGJKXHNWERSUPCT"));
  ASSERT_THAT(applySubstCipher(cipher, FOX_PANGRAM),
              StrEq("EMZ HRABD YNKUJ OKP IRGXW KSZN EMZ QVTC LKF"));
  ASSERT_THAT(applySubstCipher(cipher, MR_PANGRAM_PUNCT),
              StrEq("GN. IKBD, ES HRAT XML, YVFW OZU QCJP."));
}

class SubstEnc_RandCipherMainCommand : public CaptureCinCout {};

TEST_F(SubstEnc_RandCipherMainCommand, FullCommand) {
  // Seed prng
  input << "r" << endl;
  input << 0 << endl;

  input << "a" << endl;
  input << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << endl;

  // Must run in the same command since main re-seeds
  input << "a" << endl;
  input << MR_PANGRAM_PUNCT << endl;
  input << "x" << endl;

  ciphers_main();

  string actual_output = output.str();

  ASSERT_THAT(actual_output, HasSubstr("UHJCKSMYXWRINQALDGVFZBOEPT"))
      << "Incorrect substitution of random cipher";

  ASSERT_THAT(actual_output, HasSubstr("LZ. MXTI, OS DBPQ AFG, YEUR CWJ VNHK."))
      << "Incorrect substitution of random cipher";
}

}  // namespace
