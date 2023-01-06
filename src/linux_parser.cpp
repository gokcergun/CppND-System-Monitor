#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Parse operating system information from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
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
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
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
  std::regex r("^[0-9]*(\\.[0-9]*)?$");
  string line, key, value; 
  float memTotal {0.1}, memFree {0.1};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "MemTotal:"){
          if (std::regex_match(value, r)){
            memTotal = stof(value);
          }
        } else if (key == "MemFree:"){
          if (std::regex_match(value, r)){
           memFree = stof(value);
          }
        }
      }
    }
  }
  return (memTotal-memFree)/memTotal; 
  }

// Parse system uptime - format.cpp - later format its output to show on screen
long LinuxParser::UpTime() { 
  string line, upTime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
  return stol(upTime); 
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
  string line, token;
  long total {0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> token){
        if (i == 13 || i == 14 || i == 15 || i == 16 ){
          total += stol(token);
        } 
        i++;
    }
  }
  return total; 
  }

// Return the start time (in ticks) of the process
//REF: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::StartTimeJiffies(int pid) { 
  string line, token;
  long startTime {0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> token){
        if (i == 21){
          startTime = stol(token);
        } 
        i++;
    }
  }
  return startTime; 
  }

// Return the number of active jiffies for the system
// REF: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::ActiveJiffies(const vector<string>& cpuInfo) { 
  long activeJiffies = stol(cpuInfo[kUser_]) + stol(cpuInfo[kNice_]) + stol(cpuInfo[kSystem_]) + 
  stol(cpuInfo[kIRQ_]) + stol(cpuInfo[kSoftIRQ_]) + stol(cpuInfo[kSteal_]);
  return activeJiffies;
};

// Return the number of idle jiffies for the system
// REF: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::IdleJiffies(const vector<string>& cpuInfo) { 
  long idleJiffies = stol(cpuInfo[kIdle_])  +  stol(cpuInfo[kIOwait_]);
  return idleJiffies;
}

// Read and return CPU utilization row (top row -aggregated cpu results)
vector<string> LinuxParser::CpuUtilization() { 
  string line, key, value;
  vector<string> cpuInfo {};
  
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
  string line, key, value;
  int totalProcesses;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "processes"){
        totalProcesses = stoi(value);
        }
      }
    }
  }
  return totalProcesses; 
}

// Parse the number of running processes
int LinuxParser::RunningProcesses(){
  string line, key, value;
  int procs_running;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){ 
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "procs_running"){
        procs_running = stoi(value);
        }
      }
    }
  }
  return procs_running; 
  }

// Parse the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}


// Parse the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value, ram;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "VmSize:"){
          // convert memory from kb to mb
          ram = std::to_string(round(stof(value)/1000*100)/100);
        }
      }
    }
  }
  return ram; 
  }

// Parse the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, key, value, uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "Uid:"){
          uid = value;
        }
      }
    }
  }
  return uid; 
  }

// Parse the user associated with a process
string LinuxParser::User(int pid) { 
  string uid  = LinuxParser::Uid(pid);
  char delimiter = ':';
  string line, user;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      if (line.find(":x:" + uid) != std::string::npos){
        // extract first part until :
        size_t pos = line.find(delimiter);
        if (pos != std::string::npos) {
          user = line.substr(0, pos);
        }
        break;
      }
    }
  }
  return user; 
  }


// Parse the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  long uptime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> value){
        if (i == 21){
          uptime =stol(value);
          break;
        }
        i++;
      }
    }
  // convert uptime from ticks to second
  long tickFrequency = sysconf(_SC_CLK_TCK); // hertz
  return uptime/tickFrequency; 
  }