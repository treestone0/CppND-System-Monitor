#include "format.h"

#include <string>

using std::string;
using std::to_string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// done: shuyan
string Format::ElapsedTime(long seconds) {
  int s = seconds % 60;
  int m = (seconds / 60) % 60;
  int h = seconds / 60 / 60;
  string placeHolderM = "";
  string placeHolderS = "";
  if (m < 10) {
    placeHolderM = "0";
  }
  if (s < 10) {
    placeHolderS = "0";
  }
  return to_string(h) + ":" + placeHolderM + to_string(m) + ":" + placeHolderS +
         to_string(s);
}