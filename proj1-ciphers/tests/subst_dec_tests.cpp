#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "include/subst_dec.h"
#include "tests/test_utils.h"
#include "utils.h"

using ::testing::DoubleEq;
using ::testing::HasSubstr;
using ::testing::Values;
using ::testing::WithParamInterface;

namespace {

// The counts are copied, but the scores will be _very different_
// for the full file.
const QuadgramScorer CUSTOM_QUADGRAM_SCORER({"ABCD", "SCHO", "CHOO", "HOOL",
                                             "OOLS"},
                                            {1408, 1202417, 1140139, 104875,
                                             345451});

TEST(SubstDec_ScoreString, OneQuadgram) {
  ASSERT_THAT(scoreString(CUSTOM_QUADGRAM_SCORER, "AAAA"),
              DoubleEq(-6.4462714765326465))
      << "Incorrect score for missing quadgram";

  ASSERT_THAT(scoreString(CUSTOM_QUADGRAM_SCORER, "ABCD"),
              DoubleEq(-3.297668821726553))
      << "Incorrect score for present quadgram";
}

TEST(SubstDec_ScoreString, LongerWord) {
  ASSERT_THAT(scoreString(CUSTOM_QUADGRAM_SCORER, "SCHOOLS"),
              DoubleEq(-3.0890145687723809))
      << "Incorrect score for longer string";

  ASSERT_THAT(scoreString(CUSTOM_QUADGRAM_SCORER, "POOLSIDE"),
              DoubleEq(-26.692970928460362))
      << "Incorrect score for longer string with missing quadgrams";

  ASSERT_THAT(scoreString(CUSTOM_QUADGRAM_SCORER,
                          "SCHOOLSCHOOLSCHOOLSCHOOLSCHOOLSCHOOLSCHOOL"),
              DoubleEq(-98.070474677468624))
      << "Incorrect score for even longer string";
}

class SubstDec_EnglishnessFuncCommand : public CaptureCinCout {};

TEST_F(SubstDec_EnglishnessFuncCommand, Main) {
  input << "pool sides, by the pool side" << endl;

  computeEnglishnessCommand(CUSTOM_QUADGRAM_SCORER);

  string actual_output = output.str();
  output.clear();

  ASSERT_THAT(actual_output, HasSubstr("-111.402"))
      << "Incorrect score with custom scorer";
}

class SubstDec_EnglishnessMainCommand
    : public WithParamInterface<tuple<string, double>>,
      public CaptureCinCout {};

TEST_P(SubstDec_EnglishnessMainCommand, Case) {
  auto& param = GetParam();

  auto& commandInput = get<0>(param);
  auto expectedScore = get<1>(param);

  input << "e" << endl;
  input << commandInput << endl;
  input << "x";

  ciphers_main();

  string actual_output = output.str();
  output.clear();

  ostringstream oss;
  oss << expectedScore;

  ASSERT_THAT(actual_output, HasSubstr(oss.str()))
      << "Incorrect score with data loaded from english_quadgrams.txt";
}

INSTANTIATE_TEST_SUITE_P(
    , SubstDec_EnglishnessMainCommand,
    Values(
        // Input, score
        make_tuple("HELP", -3.79107), make_tuple("SCHOOL", -10.7195),
        make_tuple("AAAAAAAAAAAAA", -57.9934),
        make_tuple("pool sides, by the pool side", -81.5974)));

// Very, very tricky to test alone! Needs the full QuadgramScorer, which
// is loaded in main.

class SubstDec_FullMainCommand
    : public WithParamInterface<tuple<string, string>>,
      public CaptureCinCout {};

TEST_P(SubstDec_FullMainCommand, Case) {
  auto& param = GetParam();

  auto& ciphertext = get<0>(param);
  auto& plaintext = get<1>(param);

  write_to_cout(
      "This is a long test case! If you want to stop this early, press CTRL+C "
      "to terminate the test program.\n");

  input << "s" << endl;
  input << ciphertext << endl;
  input << "x";

  ciphers_main();

  string actual_output = output.str();

  ASSERT_THAT(actual_output, HasSubstr(plaintext)) << "Incorrect decryption";
}

// Since we're doing a statistical analysis, we want to use longer strings for
// reliability. This does slow things down, but it shouldn't be more than
// a minute or two.
INSTANTIATE_TEST_SUITE_P(
    , SubstDec_FullMainCommand,
    Values(
        make_tuple(
            // Ciphertext
            "PQBMKI, NBXKQ XIP XIXJVBMB RH PXLX BLTNOLNTQB: TQGMQU RH JMBLB, "
            "BLXOZB XIP ANQNQB; WXBW LXYJQB, STMRTMLV ANQNQB, BQXTOW LTQQB, "
            "MILTRPNOLMRI LR KTXSWB; BQXTOWMIK XIP BRTLMIK; TNILMCQ XIXJVBMB; "
            "STRKTXCCMIK STREQOLB XIP JXY QDQTOMBQB.",
            // Plaintext
            "DESIGN, USAGE AND ANALYSIS OF DATA STRUCTURES: REVIEW OF LISTS, "
            "STACKS AND QUEUES; HASH TABLES, PRIORITY QUEUES, SEARCH TREES, "
            "INTRODUCTION TO GRAPHS; SEARCHING AND SORTING; RUNTIME ANALYSIS; "
            "PROGRAMMING PROJECTS AND LAB EXERCISES."),
        make_tuple(
            // Ciphertext
            "TYUEPXVZ QMDD ICLP C DCYHP SPZMHA XUOTUAPAV QIPYP GUF ICLP "
            "VU SPXMSP IUQ VU ZPV VIMAHZ FT. QP QMDD YCOT FT ZDUQDG UA "
            "VIMZ; ZU, JUY VIMZ TYUEPXV, QP QMDD ZVMDD HMLP GUF C DUV "
            "UJ HFMSCAXP. IUQPLPY, MJ GUF HPV ZVFXR JUY OUYP VICA 30 "
            "OMAFVPZ UA C NFH UY FASPYZVCASMAH ZUOPVIMAH, GUF ZIUFDS "
            "XUOP VU UJJMXP IUFYZ UY CZR C KFPZVMUA UA TMCBBC. QP QCAV "
            "VIMZ PWTPYMPAXP VU NP CA PAEUGCNDP UAP, CAS AUNUSG YPCDDG "
            "PAEUGZ JYFMVDPZZ SPNFHHMAH JUY IUFYZ UA PAS.",
            // Plaintext
            "PROJECTS WILL HAVE A LARGE DESIGN COMPONENT WHERE YOU HAVE "
            "TO DECIDE HOW TO SET THINGS UP. WE WILL RAMP UP SLOWLY ON "
            "THIS; SO, FOR THIS PROJECT, WE WILL STILL GIVE YOU A LOT "
            "OF GUIDANCE. HOWEVER, IF YOU GET STUCK FOR MORE THAN 30 "
            "MINUTES ON A BUG OR UNDERSTANDING SOMETHING, YOU SHOULD "
            "COME TO OFFICE HOURS OR ASK A QUESTION ON PIAZZA. WE WANT "
            "THIS EXPERIENCE TO BE AN ENJOYABLE ONE, AND NOBODY REALLY "
            "ENJOYS FRUITLESS DEBUGGING FOR HOURS ON END."),
        make_tuple(
            // Ciphertext
            "IQL EDUAPL EWVEIDIWIDTM JDAQLO DE G JDAQLO IQGI QGE VLLM DM WEL "
            "XTO UGMR QWMFOLFE TX RLGOE. DI VGEDJGPPR JTMEDEIE TX EWVEIDIWIDMY "
            "LKLOR APGDMILCI JQGOGJILO XTO G FDXXLOLMI JDAQLOILCI JQGOGJILO. "
            "DI FDXXLOE XOTU IQL JGLEGO JDAQLO DM IQGI IQL JDAQLO GPAQGVLI DE "
            "MTI EDUAPR IQL GPAQGVLI EQDXILF, DI DE JTUAPLILPR ZWUVPLF.",
            // Plaintext
            "THE SIMPLE SUBSTITUTION CIPHER IS A CIPHER THAT HAS BEEN IN USE "
            "FOR MANY HUNDREDS OF YEARS. IT BASICALLY CONSISTS OF SUBSTITUTING "
            "EVERY PLAINTEXT CHARACTER FOR A DIFFERENT CIPHERTEXT CHARACTER. "
            "IT DIFFERS FROM THE CAESAR CIPHER IN THAT THE CIPHER ALPHABET IS "
            "NOT SIMPLY THE ALPHABET SHIFTED, IT IS COMPLETELY JUMBLED.")));

}  // namespace
