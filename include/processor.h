#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long lastActiveJiffies_;
  long lastTotalJiffies_;
};

#endif