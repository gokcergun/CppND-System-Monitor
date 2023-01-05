#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"
#include "format.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


// Return the system's CPU
Processor& System::Cpu() {  return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { 
  processes_.clear();
  std::vector<int> proccessIDs_ = LinuxParser::Pids(); 
  for (auto pid: proccessIDs_) {
    Process p(pid);
    processes_.emplace_back(p);
    }
  // sort processes according to RAM (if equal CPU Utilization)
  std::sort(processes_.begin(), processes_.end(), 
            [](const Process& p1, const Process& p2) {
              return p1 < p2;
            });
   // it sorted from small the big, we will reverse the vector
  std::reverse(processes_.begin(), processes_.end());

  return processes_; 
    }

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime() ; }
