#include <string>
#include <iostream>

#include "format.h"

using std::string;

//
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
//
string Format::ElapsedTime(long seconds) { 
  
    int hour = seconds/3600;
    int minute = seconds%3600/60;
    int sec = seconds%3600%60;
    /*
    Question: In order to be expressive about code, 
    I prefer here creating variables instead of returning them directly.
    But is it a good habit? Or do I waste memory?
    */
    return std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(sec); 
    }