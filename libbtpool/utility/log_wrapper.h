#ifndef _LOG_WRAPPER_H
#define _LOG_WRAPPER_H

#ifdef _LOGGER
#include "utility/log_filter.h"
#define  LOGID_MODEULE(moduleName)   static int s_logId = LogFilter::GetInstance()->GetLogId((moduleName));

#define LOG_ERROR if(LogFilter::GetInstance()->GetLogLevel(s_logId) <=  kLogLevelERROR) SLOG_ERROR
#define LOG_WARN if(LogFilter::GetInstance()->GetLogLevel(s_logId) <=  kLogLevelWARN) SLOG_WARN
#define LOG_INFO  if(LogFilter::GetInstance()->GetLogLevel(s_logId) <=  kLogLevelINFO) SLOG_INFO

#ifdef _DEBUG
#define LOG_DEBUG if(LogFilter::GetInstance()->GetLogLevel(s_logId) <=  kLogLevelDEBUG) SLOG_DEBUG
#define LOG_THIS_DEBUG if(LogFilter::GetInstance()->GetLogLevel(s_logId) <=  kLogLevelDEBUG) SLOG_THIS_DEBUG
#else /// _DEBUG
#define LOG_DEBUG(...)
#define LOG_THIS_DEBUG(...)
#endif /// _DEBUG


#else /// _LOGGER
#include "utility/define.h"
#include "utility/errcode.h"
#include "utility/arg.h"
#include <time.h>
#include <sys/time.h>

// static int32_t out_logger_interface(const char *fmt, ...)
// {
//     struct tm localtime;
//     struct timeval now;
//     sd_va_list ap;
//
//     sd_va_start(ap, fmt);
//     gettimeofday(&now, NULL);
//     localtime_r(&now.tv_sec, &localtime);
//
//     printf("\n[%04d-%02d-%02d %02d:%02d:%02d:%03d]: ",
//            localtime.tm_year + 1900, localtime.tm_mon + 1, localtime.tm_mday,
//            localtime.tm_hour, localtime.tm_min, localtime.tm_sec, (int)now.tv_usec / 1000);
//
//     vprintf(fmt, ap);
//
//     sd_va_end(ap);
//
//     return 0;
// }

#define LOGID_MODEULE(moduleName)
#define LOG_DEBUG(format, ...)   //out_logger_interface(  "[%s(%d)] ... " format ,__FUNCTION__,__LINE__ ,##__VA_ARGS__)
#define LOG_WARN(format, ...)
#define LOG_ERROR(format, ...)   //out_logger_interface(  "[%s(%d)] ... " format ,__FUNCTION__,__LINE__ ,##__VA_ARGS__)
#define LOG_INFO(format, ...)
#define LOG_THIS_DEBUG(format, ...)

#endif /// _LOGGER
#endif /// _LOG_WRAPPER_H
