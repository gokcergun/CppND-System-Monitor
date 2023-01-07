#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "format.h"

using std::string;

//
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
//
string Format::ElapsedTime(long sec) { 
  std::chrono::seconds seconds{sec};
  std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
  seconds -= std::chrono::duration_cast<std::chrono::seconds>(hours);
  std::chrono::minutes minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
  seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);
  
  std::stringstream outputString {};
  outputString << std::setw(2) << std::setfill('0') << hours.count() 
      		   << std::setw(1) << ":"
               << std::setw(2) << std::setfill('0') << minutes.count()
               << std::setw(1) << ":"
               << std::setw(2) << std::setfill('0') << seconds.count();
  
  return outputString.str();
}