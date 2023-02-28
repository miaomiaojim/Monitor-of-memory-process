#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  CalculateCpu();
  DetermineUser();
  DetermineCommand();
  DetermineRam();
  DetermineUptime();
};
// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUsage_; }

void Process::CalculateCpu() {
  long uptime = LinuxParser::UpTime();
  vector<float> timeCpu = LinuxParser::CpuUtilization(Pid());
  if (timeCpu.size() == 5) {
    float totaltime = timeCpu[kutime_] + timeCpu[kstime_] + timeCpu[kcutime_] + timeCpu[kcstime_];
    float seconds = uptime - timeCpu[kstarttime_];
    cpuUsage_ = totaltime / seconds;
  }
  else { cpuUsage_ = 0; }
}

// TODO: Return the command that generated this process
string Process::Command() const { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() const { return ramUsage_; }

// TODO: Return the user (name) that generated this process
string Process::User() const{ return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime_; }

void Process::DetermineUser() {
  user_ = LinuxParser::User(Pid());
}

void Process::DetermineCommand() {
  command_ = LinuxParser::Command(Pid());
}

void Process::DetermineRam() {
  string value = LinuxParser::Ram(Pid());
  try {
    // convert into MB
    long conversion = std::stol(value) / 1000;
    ramUsage_ = std::to_string(conversion);
  } catch (const std::invalid_argument& arg) {
    ramUsage_ = "0";
  }
}

void Process::DetermineUptime() {
  uptime_ = LinuxParser::UpTime(Pid());
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->cpuUsage_ < a.cpuUsage_;
}