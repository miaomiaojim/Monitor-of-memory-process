#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using namespace std::experimental::filesystem::v1;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line, key, value;
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
  string os, version, kernel;
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
  path str(kProcDirectory);
  if (!exists(str)) {
    exit(0);
  }
  directory_entry entry(str);
  if (entry.status().type() == file_type::directory) {
    directory_iterator list(str);
    for (auto it:list) {
      string name = it.path().filename();
      if (std::all_of(name.begin(), name.end(), isdigit)) {
        int pid = stoi(name);
        pids.emplace_back(pid);
      }
    }
  }
  return pids;
}
  
// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memTotal, memFree;
  string key, value, line;
  std::ifstream ramStream(kProcDirectory + kMeminfoFilename);
  if (ramStream.is_open()) {
    while (std::getline(ramStream, line)) {
      std::remove(line.begin(), line.end(), ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream ramLineStream(line);
      while (ramLineStream >> key >> value) {
        if (key == "MemTotal") {
          memTotal = std::stof(value);
        }
        else if (key == "MemFree") {
          memFree = std::stof(value);
          break;
        }
      }
    }
  }
  return ((memTotal - memFree) / memTotal);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string timeWhole;
  std::ifstream upTimeStream(kProcDirectory + kUptimeFilename);
  if (upTimeStream.is_open()) {
    while (std::getline(upTimeStream, line)) {
      std::istringstream lineUpTimeStream(line);
      while (lineUpTimeStream >> timeWhole) {
        try {
          return std::stol(timeWhole);
        }
        catch (const std::invalid_argument &invalidArg) {
          return 0;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUsage;
  string key;
  string line;
  string cpuuser, cpunice, cpusystem, cpuidle, cpuiowait, cpuirq, cpusoftirq, cpusteal, cpuguest, cpuguest_nice;
  std::ifstream cpuStream(kProcDirectory + kStatFilename);
  if (cpuStream.is_open()) {
    while (std::getline(cpuStream, line)) {
      std::istringstream lineCpuStream(line);
      while (lineCpuStream >> key >> cpuuser >> cpunice >> cpusystem >> cpuidle >> cpuiowait >> cpuirq >> cpusoftirq >> cpusteal >> cpuguest >> cpuguest_nice) {
        if (key == "cpu") {
          vector<string> cpuUsage = {cpuuser, cpunice, cpusystem, cpuidle, cpuiowait, cpuirq, cpusoftirq, cpusteal, cpuguest, cpuguest_nice};
          return cpuUsage;
        }
      }
    }
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, value, line;
  std::ifstream processStream(kProcDirectory + kStatFilename);
  if (processStream.is_open()) {
    while (std::getline(processStream, line)) {
      std::istringstream lineProcessStream(line);
      while (lineProcessStream >> key >> value) {
        if (key == "processes") {
          try {
            return std::stol(value);
          }
          catch (const std::invalid_argument &invalidarg) {
            return 0;
          }
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value, line;
  std::ifstream runningStream(kProcDirectory + kStatFilename);
  if (runningStream.is_open()) {
    while (std::getline(runningStream, line)) {
      std::istringstream lineRunningStream(line);
      while (lineRunningStream >> key >> value) {
        if (key == "procs_running") {
          try {
            return std::stol(value);
          }
          catch (const std::invalid_argument &invalidarg) {
            return 0;
          }
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string value;
  std::ifstream commandStream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (commandStream.is_open()) {
    std::getline(commandStream, value);
    return value;
  }
  return "stream error";
}

vector<float> LinuxParser::CpuUtilization(int pid) {
  vector<float> cpuUsage;
  string line, value;
  float time = 0;
  std::ifstream cpuStream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (cpuStream.is_open()) {
    while (std::getline(cpuStream, line)) {
      std::istringstream lineCpuStream(line);
      for (int i = 1; i <= starttime_index; i++) {
        lineCpuStream >> value;
        if (i == utime_index || 
            i == stime_index || 
            i == cutime_index || 
            i == cstime_index ||
            i == starttime_index) {
          time = std::stof(value) / sysconf(_SC_CLK_TCK);
          cpuUsage.emplace_back(time);
        }
      }
    }
  }
  return cpuUsage;
}

string LinuxParser::Ram(int pid) {
  string key, value, line;
  std::ifstream ramStream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (ramStream.is_open()) {
    while (std::getline(ramStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineRamstream(line);
      while (lineRamstream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  return "stream error";
}
  
// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream uidStream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (uidStream.is_open()) {
    while (std::getline(uidStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineUidStream(line);
      while (lineUidStream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return "stream error";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, key, value, other;
  // find user name for this user ID in /etc/passwd
  std::ifstream userStream(kPasswordPath);
  if (userStream.is_open()) {
    while (std::getline(userStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineUserStream(line);
      while (lineUserStream >> value >> other >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  long uptime;
  std::ifstream uptimeStream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (uptimeStream.is_open()) {
    while (std::getline(uptimeStream, line)) {
      std::istringstream lineUpTimeStream(line);
      for (int i = 1; i <= starttime_index; i++) {
        lineUpTimeStream >> value;
        if (i == starttime_index) {
          try {
            uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
            return uptime;
          } 
          catch (const std::invalid_argument &invalidarg) {
            return 0;
          }
        }
      }
    }
  }
  return 0;
}
