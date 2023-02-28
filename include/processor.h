#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class Processor {
 public:
  Processor();
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float previousTotalCpuTime;
  float previousIdleCpuTime;
  vector<long> ConvertToLong(vector<string> value);
};

#endif