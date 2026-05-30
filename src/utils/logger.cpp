#include "logger.h"
#include "../constants.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

void Logger::LogEvent(const std::string &functionName,
                      const std::string &description) {
  static std::mutex logMutex;
  std::lock_guard<std::mutex> lock(logMutex);

  const std::string logPath = "events.log";
  std::ofstream logFile(logPath, std::ios::app);
  if (!logFile.is_open()) {
    std::cerr << "Error: Unable to open log file. Message: (" << description
              << ")" << std::endl;
    return;
  }

  auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm localTime{};
#ifdef _WIN32
  localtime_s(&localTime, &now);
#else
  localtime_r(&now, &localTime);
#endif

  logFile << std::put_time(&localTime, "[%Y-%m-%d %H:%M:%S]") << " ["
          << functionName << "] " << description << "\n";
  logFile.close();

  std::error_code ec;
  if (ec || std::filesystem::file_size(logPath, ec) <= MAX_LOG_FILE_SIZE)
    return;

  std::ifstream inFile(logPath);
  if (!inFile.is_open()) {
    std::cerr << "Error: Unable to read log file for trimming!" << std::endl;
    return;
  }

  std::string fullText((std::istreambuf_iterator<char>(inFile)), {});
  inFile.close();

  while (fullText.size() > MAX_LOG_FILE_SIZE) {
    auto pos = fullText.find('\n');
    if (pos == std::string::npos)
      break;
    fullText.erase(0, pos + 1);
  }

  std::ofstream outFile(logPath, std::ios::trunc);
  if (outFile.is_open())
    outFile << fullText;
}
