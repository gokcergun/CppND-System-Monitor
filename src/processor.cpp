#include "processor.h"
#include <iostream>
#include <unistd.h>

// Calculate the aggregate CPU utilization
// REF: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() { 
  std::vector<std::string> prevCpuInfo = LinuxParser::CpuUtilization();
  previousCpuInfo_ = &prevCpuInfo;
  usleep(1000000);
  std::vector<std::string> currCpuInfo = LinuxParser::CpuUtilization();
  currentCpuInfo_ = &currCpuInfo;

  // current/previous total = ActiveJiffies + IdleJiffies
  // total = currentTotal-PreviousTotal
  long Total = (LinuxParser::ActiveJiffies(*currentCpuInfo_) + LinuxParser::IdleJiffies(*currentCpuInfo_)) 
  - (LinuxParser::ActiveJiffies(*previousCpuInfo_) + LinuxParser::IdleJiffies(*previousCpuInfo_));

  // currentIdle - previousIdled
  long Idle = LinuxParser::IdleJiffies(*currentCpuInfo_) -  LinuxParser::IdleJiffies(*previousCpuInfo_);
  float cpuUtilization = float(Total - Idle)/float(Total);
  return cpuUtilization;
  }