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
    
    return std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(sec); 
    }