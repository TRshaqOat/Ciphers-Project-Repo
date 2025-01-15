#pragma once

#include <gtest/gtest.h>

#include <ios>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <tuple>

// If we compile with GoogleTest's main, we'll need to store our own main
// somewhere in order to invoke and test it.
int ciphers_main();

class CaptureCinCout : public testing::Test {
 protected:
  std::stringstream input;
  std::stringstream output;

  std::streambuf *saved_cin{};
  std::streambuf *saved_cout{};

  CaptureCinCout() {
    saved_cin = std::cin.rdbuf();
    saved_cout = std::cout.rdbuf();
    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());
  }

  ~CaptureCinCout() {
    std::cin.rdbuf(saved_cin);
    std::cout.rdbuf(saved_cout);
    saved_cin = nullptr;
    saved_cout = nullptr;
  }

  // Debugging tool to exfiltrate despite redirection
  // Could also use printf
  void write_to_cout(std::string s) {
    saved_cout->sputn(s.c_str(), s.length());
    saved_cout->pubsync();
  }
};

template <class... Args>
auto last_elem(std::tuple<Args...> t) {
  return std::get<sizeof...(Args) - 1>(t);
}
