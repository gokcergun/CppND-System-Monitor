#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& other) const;  


 private:
 int pid_;
 std::string user_;
 std::string command_;
 float cpuUtilization_;
 std::string ram_;
 long upTime_;

 // declare setters
 void User(int pid);                      
 void Command(int pid);                   
 void CpuUtilization(int pid);            
 void Ram(int pid);                       
 void UpTime(int pid);

 
};

#endif