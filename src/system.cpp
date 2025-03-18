#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

// Return the system's CPU
// done: shuyan
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
// done: shuyan
vector<Process>& System::Processes() {
  updateProcesses();
  return processes_;
}

// Return the system's kernel identifier (string)
// done: shuyan
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
// done: shuyan
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
// done: shuyan
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
// done: shuyan
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
// done: shuyan
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
// done: shuyan
long int System::UpTime() { return LinuxParser::UpTime(); }

void System::updateProcesses() {
  auto pids = LinuxParser::Pids();
  processes_.clear();
  for (auto pid : pids) {
    Process process(pid);
    processes_.emplace_back(process);
  }
  std::sort(processes_.begin(), processes_.end());
}