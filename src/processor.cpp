#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
// done: shuyan
float Processor::Utilization() {
  long accumulatedTotalJiffies = LinuxParser::Jiffies();
  long accumulatedActiveJiffies = LinuxParser::ActiveJiffies();

  long activeJiffiesDelta = accumulatedActiveJiffies - lastActiveJiffies_;
  long totalJiffiesDelta = accumulatedTotalJiffies - lastTotalJiffies_;

  lastActiveJiffies_ = accumulatedActiveJiffies;
  lastTotalJiffies_ = accumulatedTotalJiffies;

  return (float)activeJiffiesDelta / totalJiffiesDelta;
}