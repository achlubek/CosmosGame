#include "stdafx.h"
using namespace std;


DebugLogger::DebugLogger()
{
}


DebugLogger::~DebugLogger()
{
}

void DebugLogger::setDesiredSeverityThreshold(LogSeverity threshold)
{
    desiredSeverityThreshold = threshold;
}

void DebugLogger::log(LogSeverity severity, std::string string)
{
    if (static_cast<int>(severity) <= static_cast<int>(desiredSeverityThreshold)) {
        time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        struct tm buf;
        char str[64];
        localtime_s(&buf, &now);
        std::cout << "[" << put_time(&buf, "%F %T") << "]: " << string << endl;
    }
}
