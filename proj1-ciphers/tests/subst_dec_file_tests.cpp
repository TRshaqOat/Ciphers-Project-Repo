#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "tests/test_utils.h"

using ::std::endl;
using ::std::ifstream;
using ::std::string;
using ::std::stringstream;
using ::testing::StrEq;

class SubstDecFile_MainCommand : public CaptureCinCout {};

TEST_F(SubstDecFile_MainCommand, FireIce) {
  write_to_cout(
      "This is a long test case! If you want to stop this early, press CTRL+C "
      "to terminate the test program.\n");

  string input_file = "test_data/fire_ice_enc.txt";
  string output_file = "actual_fire_ice.txt";
  input << "f" << endl;
  input << input_file << endl;
  input << output_file << endl;
  input << "x" << endl;

  ciphers_main();

  ifstream f_expected("test_data/fire_ice_dec.txt");
  ASSERT_TRUE(f_expected.good())
      << "Something's wrong with the test data. Are you running this "
         "from the correct directory?";
  stringstream ss_expected;
  ss_expected << f_expected.rdbuf();

  ifstream f_actual(output_file);
  ASSERT_TRUE(f_actual.good()) << "Couldn't open output file given to command";

  // Retrieve data, delete output file
  stringstream ss_actual;
  ss_actual << f_actual.rdbuf();
  f_actual.close();
  std::filesystem::remove(output_file);

  ASSERT_THAT(ss_actual.str(), StrEq(ss_expected.str()));
}
