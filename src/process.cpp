#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
#include <iostream>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid_(pid) {
    CpuUtilization(pid_);
    Command(pid_);
    Ram(pid_);
    User(pid_);
    UpTime(pid_);
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Setter and Getter CpuUtilization of the process
void Process::CpuUtilization(int pid) { 
    long jeffies = LinuxParser::Jiffies();
    long startTime = LinuxParser::StartTimeJiffies(pid);
    long total = LinuxParser::ActiveJiffies(pid);
    cpuUtilization_ = float(total)/float(jeffies-startTime);
    }

float Process::CpuUtilization() { return cpuUtilization_;}

// Setter and Getter for the command that generated this process
void Process::Command(int pid) { command_ = LinuxParser::Command(pid_);}
string Process::Command() { return command_; }

// Setter and Getter for this process's memory utilization
void Process::Ram(int pid) { ram_ = LinuxParser::Ram(pid_); }
string Process::Ram() { return ram_; }

// Setter and Getter for the user (name) that generated this process
void Process::User(int pid) { user_ = LinuxParser::User(pid_); }
string Process::User() { return user_; }

// Setter and Getter the age of this process (in seconds)
void Process::UpTime(int pid) { upTime_ = LinuxParser::UpTime(pid_); }
long int Process::UpTime() { return upTime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const { 
    // compare self with the other (lhand of < )
    if (std::stof(ram_) == std::stof(other.ram_)){
        return cpuUtilization_ < other.cpuUtilization_;
    }
    return std::stof(ram_) < std::stof(other.ram_);
}