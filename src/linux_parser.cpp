#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// Read and return the system memory utilization
// done: shuyan
/*
  according to the following link:
  https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
    Total used memory = MemTotal - MemFree
    Non cache/buffer memory (green) = Total used memory - (Buffers + Cached
  memory) Buffers (blue) = Buffers Cached memory (yellow) = Cached +
  SReclaimable - Shmem Swap = SwapTotal - SwapFree
*/
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;

  int totalMemory = 0;
  int freeMemory = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        totalMemory = stoi(value);
      }
      if (key == "MemFree:") {
        freeMemory = stoi(value);
      }
    }
  }
  return (float)(totalMemory - freeMemory) / totalMemory;
}

// Read and return the system uptime
// done: shuyan
long LinuxParser::UpTime() {
  string upTime, idleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }
  return stol(upTime);
}

// Read and return the number of jiffies for the system
// done: shuyan
/*
  according to the following link:
  https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // Guest time is already accounted in usertime
  usertime = usertime - guest;                             # As you see here, it
  subtracts guest from user time nicetime = nicetime - guestnice; # and
  guest_nice from nice time
  // Fields existing on kernels >= 2.6
  // (and RHEL's patched kernel 2.4...)
  unsigned long long int idlealltime = idletime + ioWait;  # ioWait is added in
  the idleTime unsigned long long int systemalltime = systemtime + irq +
  softIrq; unsigned long long int virtalltime = guest + guestnice; unsigned long
  long int totaltime = usertime + nicetime + systemalltime + idlealltime + steal
  + virtalltime;
*/
long LinuxParser::Jiffies() {
  string line;
  string key;
  string userTime;
  string niceTime;
  string systemTime;
  string idleTime;
  string ioWait;
  string irq;
  string softIrq;
  string steal;
  string guest;
  string guestNice;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> userTime >> niceTime >> systemTime >> idleTime >>
          ioWait >> irq >> softIrq >> steal >> guest >> guestNice;
      if (key == "cpu") {
        unsigned long long int idleAllTime = stoull(idleTime) + stoull(ioWait);
        unsigned long long int systemAllTime =
            stoull(systemTime) + stoull(irq) + stoull(softIrq);
        unsigned long long int virtAllTime = stoull(guest) + stoull(guestNice);
        unsigned long long int totalTime = stoull(userTime) + stoull(niceTime) +
                                           systemAllTime + idleAllTime +
                                           stoull(steal) + virtAllTime;

        return totalTime;
      }
    }
  }
  return 0;
}

// Read and return the number of active jiffies for a PID
// done: shuyan
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  vector<string> values;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    string value;
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  // std::ofstream outfile("output.txt", std::ios::app);
  // outfile << pid << " UpTime: " << values[21] << std::endl;

  // #14 utime - CPU time spent in user code, measured in clock ticks
  // #15 stime - CPU time spent in kernel code, measured in clock ticks
  // #16 cutime - Waited-for children's CPU time spent in user code (in clock
  // ticks) #17 cstime - Waited-for children's CPU time spent in kernel code (in
  // clock ticks)
  if (values.size() < 17) {
    return 0;
  }

  auto utime = stoull(values[13]);
  auto stime = stoull(values[14]);
  auto cutime = stoull(values[15]);
  auto cstime = stoull(values[16]);

  return utime + stime + cutime + cstime;
}

// Read and return the number of active jiffies for the system
// done: shuyan
long LinuxParser::ActiveJiffies() {
  string line;
  string key;
  string userTime;
  string niceTime;
  string systemTime;
  string idleTime;
  string ioWait;
  string irq;
  string softIrq;
  string steal;
  string guest;
  string guestNice;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> userTime >> niceTime >> systemTime >> idleTime >>
          ioWait >> irq >> softIrq >> steal >> guest >> guestNice;
      if (key == "cpu") {
        unsigned long long int idleAllTime = stoull(idleTime) + stoull(ioWait);
        unsigned long long int systemAllTime =
            stoull(systemTime) + stoull(irq) + stoull(softIrq);
        unsigned long long int virtAllTime = stoull(guest) + stoull(guestNice);
        unsigned long long int totalTime = stoull(userTime) + stoull(niceTime) +
                                           systemAllTime + idleAllTime +
                                           stoull(steal) + virtAllTime;

        return totalTime - idleAllTime;
      }
    }
  }
  return 0;
}

// Read and return the number of idle jiffies for the system
// done: shuyan
long LinuxParser::IdleJiffies() {
  string line;
  string key;
  string idleTime;
  string ioWait;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string temp;
      linestream >> key >> temp >> temp >> temp >> idleTime >> ioWait >> temp >>
          temp >> temp >> temp >> temp;
      if (key == "cpu") {
        unsigned long long int idleAllTime = stoull(idleTime) + stoull(ioWait);
        return idleAllTime;
      }
    }
  }
  return 0;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Read and return the total number of processes
// done: shuyan
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
// done: shuyan
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  // std::ofstream outfile("output.txt");
  if (filestream.is_open()) {
    // outfile << "filestream is open" << std::endl;
    while (std::getline(filestream, line)) {
      // outfile << "line: " << line << std::endl;

      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        // outfile << "key: " << key << std::endl;
        // outfile << "value: " << value << std::endl;
        return stoi(value);
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
// done: shuyan
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

// Read and return the memory used by a process
// done: shuyan
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        // std::ofstream outfile("output.txt", std::ios::app);
        // outfile << pid << " VmSize: " << value << std::endl;
        return to_string(stoi(value) / 1024);
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
// done: shuyan
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        // std::ofstream outfile("output.txt", std::ios::app);
        // outfile << pid << " Uid: " << value << std::endl;
        return value;
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
// done: shuyan
string LinuxParser::User(int pid) {
  auto uid = Uid(pid);
  string line;
  string name;
  string id;
  string temp;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');

      // std::ofstream outfile("output.txt", std::ios::app);
      // outfile << pid << " User: " << line << std::endl;

      std::istringstream linestream(line);
      linestream >> name >> temp >> id;
      if (id == uid) {
        // std::ofstream outfile("output.txt", std::ios::app);
        // outfile << pid << " User: " << name << std::endl;
        return name;
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
// done: shuyan
long LinuxParser::UpTime(int pid) {
  string line;
  vector<string> values;
  string value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  // std::ofstream outfile("output.txt", std::ios::app);
  // outfile << pid << " UpTime: " << values[21] << std::endl;

  // #22 starttime - Time when the process started, measured in clock ticks
  if (values.size() < 22) {
    return 0;
  }
  return stol(values[21]) / sysconf(_SC_CLK_TCK);
}
