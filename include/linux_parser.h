#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>


namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};
  
//regex pattern to check numbers
const std::regex kNumberPattern("^[0-9]*(\\.[0-9]*)?$");

// Keys to access values
const std::string kProcessesKey("processes");
const std::string kRunningProcessesKey("procs_running");
const std::string kMemTotalKey("MemTotal:");
const std::string kMemFreeKey("MemFree:");
const std::string kUIDKey("Uid:");
const std::string kProcMemKey("VmRSS:");

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_ ,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies(int pid);
long StartTimeJiffies(int pid);
long ActiveJiffies(const std::vector<std::string>& cpuInfo);
long IdleJiffies(const std::vector<std::string>& cpuInfo);

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
  
template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename);
template <typename T>
T getValueOfFile(std::string const &filename);
};  // namespace LinuxParser

#endif