#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::vector;

Processor::Processor() : previousTotalCpuTime(0.0), previousIdleCpuTime(0.0) {};

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<long> valueCpu = ConvertToLong(LinuxParser::CpuUtilization());
  // total cpu time since boot = user+nice+system+idle+iowait+irq+softirq+steal
  float totalCpuTime =
      valueCpu[LinuxParser::user_] + 
      valueCpu[LinuxParser::nice_] +
      valueCpu[LinuxParser::system_] + 
      valueCpu[LinuxParser::idle_] +
      valueCpu[LinuxParser::iOwait_] + 
      valueCpu[LinuxParser::iRQ_] +
      valueCpu[LinuxParser::softIRQ_] + 
      valueCpu[LinuxParser::steal_];
  // idle time since boot = idle + iowait
  float idleCpuTime =
      valueCpu[LinuxParser::idle_] + valueCpu[LinuxParser::iOwait_];
  // calculate the cpu usage since last update
  float differenceIdle = idleCpuTime - previousIdleCpuTime;
  float differenceTotal = totalCpuTime - previousTotalCpuTime;
  float differenceUsage = (differenceTotal - differenceIdle) / differenceTotal;
  // remember the total and idle times for next check
  previousIdleCpuTime = idleCpuTime;
  previousTotalCpuTime = totalCpuTime;
  return differenceUsage;
}

vector<long> Processor::ConvertToLong(vector<string> value) {
  vector<long> valueConverted;
  for (int it = 0; it < (int)value.size(); it++) {
    try {
      valueConverted.emplace_back(std::stol(value[it]));
    } 
    catch (const std::invalid_argument& arg) {
      valueConverted.emplace_back((long)0);
    }
  }
  return valueConverted;
}