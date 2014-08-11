#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#include <string>

#include "ace/Log_Msg.h"

std::string dataLoc (const std::string& filename, bool doCheck = true);
std::string alterPathForPlatform (char *filename);
void printExtensions (FILE *fstream, const char* extstr_in);

bool initLogging(const std::string& programName_in,
                 const std::string& logFile_in,
                 bool logToSyslog_in,
                 bool enableTracing_in,
                 bool enableDebug_in,
                 ACE_Log_Msg_Backend* backend_in);

#endif // COMMON_H
