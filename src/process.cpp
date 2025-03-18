#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
// done: shuyan
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
// done: shuyan
float Process::CpuUtilization() const {
  auto processActiveJiffies = LinuxParser::ActiveJiffies(pid_);
  auto processActiveTime = (float)processActiveJiffies / sysconf(_SC_CLK_TCK);

  auto processUpTime = LinuxParser::UpTime(pid_);
  auto systemUpTime = LinuxParser::UpTime();
  auto processRunTime = systemUpTime - processUpTime;

  // std::ofstream outfile("output.txt", std::ios::app);
  // outfile << pid_ << " UpTime: " << processUpTime
  //         << " SystemUpTime: " << systemUpTime
  //         << " ProcessRunTime: " << processRunTime
  //         << " ProcessActiveTime: " << processActiveTime << std::endl;
  return (processRunTime > 0) ? (processActiveTime / processRunTime) : 0.0f;
}

// Return the command that generated this process
// done: shuyan
string Process::Command() const { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
// done: shuyan
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
// done: shuyan
string Process::User() const { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
// done: shuyan
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
// done: shuyan
bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < this->CpuUtilization();
}