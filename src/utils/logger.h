#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
  static void LogEvent(const std::string &functionName,
                       const std::string &description);
};

#define LOG_EVENT(desc) Logger::LogEvent(__FUNCTION__, desc)

#endif
