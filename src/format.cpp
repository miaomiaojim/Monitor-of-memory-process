#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string time = "00:00:00";
  long hour, minute, second;
  hour = seconds / 3600;
  minute = seconds / 60 % 60;
  second = seconds % 60;
  time = TimeToString(hour) + ":" + TimeToString(minute) + ":" + TimeToString(second);
  return time;
}

string Format::TimeToString(long time) {
  if (time < 10) {
    return "0" + std::to_string(time);
  }
  else {
    return std::to_string(time);
  }
}