#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "tests/test_utils.h"

using ::testing::ContainerEq;
using ::testing::ContainsRegex;
using ::testing::ElementsAreArray;
using ::testing::EndsWith;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::StrEq;
using ::testing::TestParamInfo;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::WithParamInterface;

namespace {

TEST(CaesarDec_RotVec, Empty) {
  vector<string> input;
  rot(input, 10);

  ASSERT_THAT(input, IsEmpty());
}

TEST(CaesarDec_RotVec, OneElem) {
  vector<string> input = {"ABCD"};

  // Make sure that we didn't make a new vector! We want to reuse the existing
  // vector, but the reference allows us to completely reassign. We do this by
  // checking that that the underlying data is at the same place.
  string* start = input.data();

  rot(input, 1);

  ASSERT_THAT(input.data(), Eq(start))
      << "The vector itself was reassigned. Make sure "
         "you only modify its elements!";
  ASSERT_THAT(input, ElementsAreArray({"BCDE"}));
}

TEST(CaesarDec_RotVec, MultipleElems) {
  vector<string> input = {"ABCD", "DDDD", "ZZZZ"};
  rot(input, 1);

  ASSERT_THAT(input, ElementsAreArray({"BCDE", "EEEE", "AAAA"}));
}

class CaesarDec_Clean : public TestWithParam<tuple<string, string>> {};

TEST_P(CaesarDec_Clean, Case) {
  auto& param = GetParam();
  auto& input = get<0>(param);
  auto& expected = get<1>(param);

  ASSERT_THAT(clean(input), StrEq(expected));
}

INSTANTIATE_TEST_SUITE_P(, CaesarDec_Clean,
                         Values(make_tuple("", ""),
                                make_tuple("UPPERCASELETTERS",
                                           "UPPERCASELETTERS"),
                                make_tuple("1234567890!@#$%^&*() .,", ""),
                                make_tuple("a-b C", "ABC")));

class CaesarDec_SplitSpaces
    : public TestWithParam<tuple<string, vector<string>>> {};

TEST_P(CaesarDec_SplitSpaces, Case) {
  auto& param = GetParam();
  auto& input = get<0>(param);
  auto& expected = get<1>(param);

  ASSERT_THAT(splitBySpaces(input), ElementsAreArray(expected));
}

// Can't use bare brace-init. Values accepts template parameters, and the
// compiler doesn't know what type this should become!
INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_SplitSpaces,
    Values(make_tuple("", vector<string>{}),
           make_tuple("HI", vector<string>{"HI"}),
           make_tuple("HI THERE", vector<string>{"HI", "THERE"}),
           make_tuple("A B C DDD", vector<string>{"A", "B", "C", "DDD"}),
           make_tuple("  LEADING AND TRAILING SPACES  ",
                      vector<string>{"LEADING", "AND", "TRAILING", "SPACES"}),
           make_tuple("  SPACES  ", vector<string>{"SPACES"}),
           make_tuple("MULTIPLE    SPACES     BETWEEN",
                      vector<string>{"MULTIPLE", "SPACES", "BETWEEN"})));

class CaesarDec_JoinWithSpaces
    : public TestWithParam<tuple<vector<string>, string>> {};

TEST_P(CaesarDec_JoinWithSpaces, Case) {
  auto& param = GetParam();
  auto& input = get<0>(param);
  auto& expected = get<1>(param);

  ASSERT_THAT(joinWithSpaces(input), StrEq(expected));
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_JoinWithSpaces,
    Values(make_tuple(vector<string>{}, ""),
           make_tuple(vector<string>{"WORD"}, "WORD"),
           make_tuple(vector<string>{"MORE", "WORDS"}, "MORE WORDS"),
           make_tuple(vector<string>{"LOOK", "AT", "ALL", "THESE", "WORDS"},
                      "LOOK AT ALL THESE WORDS")));

class CaesarDec_NumWordsIn
    : public TestWithParam<tuple<vector<string>, vector<string>, int>> {};

TEST_P(CaesarDec_NumWordsIn, Case) {
  auto& param = GetParam();
  auto& input = get<0>(param);
  auto& dict = get<1>(param);
  auto& expected = get<2>(param);

  ASSERT_THAT(numWordsIn(input, dict), Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_NumWordsIn,
    // Text, dictionary, expected count
    Values(make_tuple(vector<string>{"WORD"}, vector<string>{}, 0),
           make_tuple(vector<string>{"WORD", "NOTWORD"}, vector<string>{"WORD"},
                      1),
           make_tuple(vector<string>{"WORD", "WORD", "NOTWORD"},
                      vector<string>{"WORD"}, 2),
           make_tuple(vector<string>{"W", "O", "R", "D"},
                      vector<string>{"A", "B", "C", "D", "E", "F", "G", "H",
                                     "R", "W"},
                      3)));

// These tests use a custom dictionary, since they call the command directly.

class CaesarDec_CustomDictCommand
    : public WithParamInterface<tuple<string, vector<string>, string, string>>,
      public CaptureCinCout {};

const string NO_GOOD_DEC = "No good decryptions found";

TEST_P(CaesarDec_CustomDictCommand, Case) {
  auto& param = GetParam();
  auto& commandInput = get<0>(param);
  auto& dict = get<1>(param);
  auto& expected_decs = get<2>(param);

  input << commandInput << endl;

  caesarDecryptCommand(dict);

  string actual_output = output.str();

  for (int i = 0; i < 26; i++) {
    string rot_candidate = rot(commandInput, i);
    if (expected_decs.find(rot_candidate) != string::npos) {
      ASSERT_THAT(actual_output, HasSubstr(rot_candidate))
          << "Missing expected decryption with rotation " << i;
    } else {
      ASSERT_THAT(actual_output, Not(HasSubstr(rot_candidate)))
          << "Output decryption with rotation " << i
          << " that doesn't met threshold";
    }
  }

  ASSERT_THAT(actual_output, HasSubstr(expected_decs))
      << "Incorrect order of decryptions with custom dictionary.";
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_CustomDictCommand,
    Values(make_tuple("aaaaaaaaa", vector<string>{}, NO_GOOD_DEC, "NoResults"),
           make_tuple("hello", vector<string>{"HELLO"}, "HELLO",
                      "OneWordOneResult"),
           make_tuple("nk", vector<string>{"BY", "HE", "IF", "NA"},
                      "BY\nHE\nIF", "OneWordManyResults"),
           make_tuple("hwukqp", vector<string>{"LAYOUT", "FUSION"},
                      "LAYOUT\nFUSION", "OneWordVerifyOrder"),
           make_tuple("or", vector<string>{"BE", "OR"}, "OR\nBE",
                      "OneWordVerifyStartFromZero"),
           make_tuple("ai   nyug", vector<string>{"GO", "TEAM"}, "GO TEAM",
                      "TwoWordsOneResult"),
           make_tuple("FYYFHP FY IFBS", vector<string>{"AT", "ATTACK"},
                      "ATTACK AT DAWN", "ThreeWordsNeedsAtLeastTwo"),
           make_tuple("ai   nyug", vector<string>{"OW", "TEAM", "WE"},
                      NO_GOOD_DEC, "TwoWordsNeedsBothInDict"),
           make_tuple(
               "DOLU PU AOL JVBYZL VM OBTHU LCLUAZ PA ILJVTLZ ULJLZZHYF MVY "
               "VUL WLVWSL AV KPZZVSCL AOL "
               "WVSPAPJHS IHUKZ DOPJO OHCL JVUULJALK AOLT DPAO HUVAOLY HUK AV "
               "HZZBTL HTVUN AOL WVDLYZ VM AOL "
               "LHYAO AOL ZLWHYHAL HUK LXBHS ZAHAPVU AV DOPJO AOL SHDZ VM "
               "UHABYL HUK VM UHABYLZ NVK LUAPASL "
               "AOLT H KLJLUA YLZWLJA AV AOL VWPUPVUZ VM THURPUK YLXBPYLZ AOHA "
               "AOLF ZOVBSK KLJSHYL AOL JHBZLZ "
               "DOPJO PTWLS AOLT AV AOL ZLWHYHAPVU",
               vector<string>{
                   "A",        "AMONG",     "AND",    "ANOTHER",  "ASSUME",
                   "BECOMES",  "CONNECTED", "COURSE", "DECLARE",  "DISSOLVE",
                   "EARTH",    "EQUAL",     "EVENTS", "FOR",      "HAVE",
                   "HUMAN",    "IN",        "IT",     "LAWS",     "NECESSARY",
                   "OF",       "ONE",       "PEOPLE", "REQUIRES", "RESPECT",
                   "SEPARATE", "SHOULD",    "THAT",   "THE",      "THEM",
                   "THEY",     "TO",        "WHEN",   "WHICH",    "WITH"},
               "WHEN IN THE COURSE OF HUMAN EVENTS IT BECOMES NECESSARY FOR "
               "ONE PEOPLE TO DISSOLVE THE "
               "POLITICAL BANDS WHICH HAVE CONNECTED THEM WITH ANOTHER AND TO "
               "ASSUME AMONG THE POWERS OF THE EARTH THE "
               "SEPARATE AND EQUAL STATION TO WHICH THE LAWS OF NATURE AND OF "
               "NATURES GOD ENTITLE THEM A DECENT RESPECT TO "
               "THE OPINIONS OF MANKIND REQUIRES THAT THEY SHOULD DECLARE THE "
               "CAUSES WHICH IMPEL THEM TO THE SEPARATION",
               "LongStressTest")),
    [](const TestParamInfo<CaesarDec_CustomDictCommand::ParamType>& info) {
      return last_elem(info.param);
    });

// These use the full dictionary.txt because they pass through main!

class CaesarDec_FullDictMainCommand
    : public WithParamInterface<tuple<string, string, string>>,
      public CaptureCinCout {};

TEST_P(CaesarDec_FullDictMainCommand, Case) {
  auto& param = GetParam();
  auto& commandInput = get<0>(param);
  auto& expected_decs = get<1>(param);

  input << "d" << endl;
  input << commandInput << endl;
  input << "x";

  ciphers_main();

  string actual_output = output.str();

  for (int i = 0; i < 26; i++) {
    string rot_candidate = rot(commandInput, i);
    if (expected_decs.find(rot_candidate) != string::npos) {
      ASSERT_THAT(actual_output, HasSubstr(rot_candidate))
          << "Missing expected decryption with rotation " << i;
    } else {
      ASSERT_THAT(actual_output, Not(HasSubstr(rot_candidate)))
          << "Output decryption with rotation " << i
          << " that doesn't met threshold";
    }
  }

  ASSERT_THAT(actual_output, HasSubstr(expected_decs))
      << "Incorrect decryptions or incorrect order of decryptions with "
         "dictionary.txt.";
}

INSTANTIATE_TEST_SUITE_P(
    , CaesarDec_FullDictMainCommand,
    Values(make_tuple("hello", "HELLO", "OneWordOneResult"),
           make_tuple("hwukqp", "LAYOUT\nFUSION", "OneWordTwoResults"),
           make_tuple("zomcih", "FUSION\nLAYOUT", "OneWordTwoResultsAgain"),
           make_tuple("iwxh xh paa p wjvt bxhjcstghipcsxcv",
                      "THIS IS ALL A HUGE MISUNDERSTANDING", "LongerResult"),
           make_tuple("FY IFBS", NO_GOOD_DEC, "Failure")),
    [](const TestParamInfo<CaesarDec_FullDictMainCommand::ParamType>& info) {
      return last_elem(info.param);
    });

}  // namespace
