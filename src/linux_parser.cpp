#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

#include "linux_parser.h"


template <typename T>
/*Question:which is better? 
Defining this template functions under LinuxParser namspace or outside of it?
*/
T LinuxParser::findValueByKey(std::string const &keyFilter, std::string const &filename){
  std::string line, key;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == keyFilter){ 
          stream.close();
          return value;
          }
      }
    }
  }
  return value;
}

template <typename T>
T LinuxParser::getValueOfFile(std::string const &filename){
  std::string line;
  T value;
  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  stream.close();
  return value;
  }

// Parse operating system information from the filesystem
std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Parse kernel information from the filesystem
std::string LinuxParser::Kernel() {
  std::string os, kernel, version;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Parse memory information of system and calculate memory utilization
float LinuxParser::MemoryUtilization() { 
  float Total {0.1}, Free {0.1};

  std::string totalValue = findValueByKey<std::string>(kMemFreeKey, kMeminfoFilename);
  if (std::regex_match(totalValue, kNumberPattern)){ Total = stof(totalValue);}
  std::string freeValue = findValueByKey<std::string>(kMemFreeKey, kMeminfoFilename);
  if (std::regex_match(freeValue, kNumberPattern)){ Free = stof(freeValue);}

  return (Total-Free)/Total; 
  }

// Parse system uptime - format.cpp - later format its output to show on screen
long LinuxParser::UpTime() { 
  std::string upTime;
  long uptime_l {0};
  upTime = getValueOfFile<std::string>(kUptimeFilename);
  if (std::regex_match(upTime, kNumberPattern)) { uptime_l = stol(upTime);}
  
  return uptime_l; 
  }

// Return the number of ticks since the last system boot - (Number of seconds since the boot * Hertz)
// REF: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::Jiffies() { 
  long tickFrequency = sysconf(_SC_CLK_TCK); // hertz
  return LinuxParser::UpTime() * tickFrequency; 
  }

// Read and return the number of active jiffies for a PID
//REF: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) { 
  std::string line, token;
  long total {0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> token){
        if (i == 13 || i == 14 || i == 15 || i == 16 ){
          if (std::regex_match(token, kNumberPattern)) {total += stol(token);}
        } 
        i++;
    }
  }
  return total; 
  }

// Return the start time (in ticks) of the process
//REF: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::StartTimeJiffies(int pid) { 
  std::string line, token;
  long startTime {0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> token){
        if (i == 21){
           if (std::regex_match(token, kNumberPattern)) { startTime = stol(token);}
        } 
        i++;
    }
  }
  return startTime; 
  }

// Return the number of active jiffies for the system
// REF: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::ActiveJiffies(const std::vector<std::string>& cpuInfo) { 
  long activeJiffies = std::stol(cpuInfo[kUser_]) + std::stol(cpuInfo[kNice_]) + std::stol(cpuInfo[kSystem_]) + 
  std::stol(cpuInfo[kIRQ_]) + std::stol(cpuInfo[kSoftIRQ_]) + std::stol(cpuInfo[kSteal_]);
  return activeJiffies;
};

// Return the number of idle jiffies for the system
// REF: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::IdleJiffies(const std::vector<std::string>& cpuInfo) { 
  long idleJiffies = std::stol(cpuInfo[kIdle_])  +  std::stol(cpuInfo[kIOwait_]);
  return idleJiffies;
}

// Read and return CPU utilization row (top row -aggregated cpu results)
std::vector<std::string> LinuxParser::CpuUtilization() { 
  std::string line, key, value;
  std::vector<std::string> cpuInfo {};
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line); // read first line - the key if cpu values are: user-nice-system-idle-iowait-irq-softirq-teal-guest-guest_nice
    std::istringstream linestream(line);
    linestream >> key; //read key cpu
    while (linestream >> value){
      cpuInfo.push_back(value);
    }
  }
  // check the size of vector
  assert(cpuInfo.size() == 10);
  return cpuInfo;
  }
  
// Parse the total number of processes
int LinuxParser::TotalProcesses() { 
  std::string processesValue;
  int totalProcesses {0};

  processesValue = findValueByKey<std::string>(kProcessesKey, kStatFilename);
  if (std::regex_match(processesValue, kNumberPattern)) {totalProcesses = stoi(processesValue);}
  return totalProcesses; 
}

// Parse the number of running processes
int LinuxParser::RunningProcesses(){
  std::string runningProcessesValue;
  int runningProcesses {0};

  runningProcessesValue = findValueByKey<std::string>(kRunningProcessesKey, kStatFilename);
  if (std::regex_match(runningProcessesValue, kNumberPattern)) {runningProcesses = stoi(runningProcessesValue);}
  return runningProcesses; 
  }

// Parse the command associated with a process
std::string LinuxParser::Command(int pid) { 
  return getValueOfFile<std::string>(std::to_string(pid) + kCmdlineFilename);
}

// Parse the memory used by a process
std::string LinuxParser::Ram(int pid) { 
  std::string line, key, value;
  std::string ram {"0"};
  float ram_float {0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        // I am using VmRSS value instead of VmSize
        if (key == kProcMemKey){
          // convert memory from kb to mb
           if (std::regex_match(value, kNumberPattern)) { ram_float = stof(value);}
           ram = std::to_string(round(ram_float/1000*100)/100);
        }
      }
    }
  }
  return ram; 
  }

// Parse the user ID associated with a process
std::string LinuxParser::Uid(int pid) { 
  std::string uid;
  std::string filename = std::to_string(pid) + kStatusFilename;
  uid = findValueByKey<std::string>(kUIDKey, filename);
  return uid; 
  }

// Parse the user associated with a process
std::string LinuxParser::User(int pid) { 
  std::string uid  = LinuxParser::Uid(pid);
  char delimiter = ':';
  std::string line, user;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      if (line.find(":x:" + uid) != std::string::npos){
        // extract first part until :
        size_t pos = line.find(delimiter);
        if (pos != std::string::npos) { user = line.substr(0, pos);}
        break;
      }
    }
  }
  return user; 
  }


// Parse the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line, value;
  long uptime {0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> value){
        if (i == 21){
          if (std::regex_match(value, kNumberPattern)) { uptime =stol(value); }
          break;
        }
        i++;
      }
    }
  // convert uptime from ticks to second
  long tickFrequency = sysconf(_SC_CLK_TCK); // hertz
  return uptime/tickFrequency; 
  }