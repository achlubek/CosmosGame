#pragma once
enum class LogSeverity {
    Important = 0,
    Normal = 1,
    Trivial = 2,
    Verbose = 3
};

class DebugLogger
{
public:
    DebugLogger();
    ~DebugLogger();
    void setDesiredSeverityThreshold(LogSeverity threshold);
    void log(LogSeverity severity, std::string string);
private:
    LogSeverity desiredSeverityThreshold = LogSeverity::Normal;
};

