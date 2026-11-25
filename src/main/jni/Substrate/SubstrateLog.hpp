
#ifndef SUBSTRATE_LOG_HPP
#define SUBSTRATE_LOG_HPP

#if 0
#include <android/log.h>

#define MSLog(level, format, ...)

#define MSLogLevelNotice ANDROID_LOG_INFO
#define MSLogLevelWarning ANDROID_LOG_WARN
#define MSLogLevelError ANDROID_LOG_ERROR

#else

#define MSLog(level, format, ...)

#endif

#endif
