#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();  

  
 private:
 /*
 QUESTION: Here is it better to define as referance and pointer?
 */
 std::vector<std::string>* previousCpuInfo_ ;
 std::vector<std::string>* currentCpuInfo_ ;
 
};

#endif