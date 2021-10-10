#ifndef FCLOG_FCLOG_H
#define FCLOG_FCLOG_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#define LOG_BUFFER_SIZE 256
#define FCDEF static inline
#define FcLog_debug(format, ...) FcLog_cmd(FC_LOG_DEBUG, __FILE__, __LINE__, format, __VA_ARGS__)

#if _M_X64
#include "FcLog_X86.h"
#define FcLog_MSR FcLog_MSR()
#else
#define FcLog_MSR 0
#endif

static const char *s_FcLog_format[] = {

        "%02d.%02d.%04d %02d:%02d:%02d \x1b[33mDEBUG \x1b[0m%s:%d: ",
        "%02d.%02d.%04d %02d:%02d:%02d DEBUG %s:%d: ",
};
enum {
    FC_LOG_NO_ERROR,
    FC_LOG_ERROR_INPUT,
    FC_LOG_TIME_NULL,
    FC_LOG_LOCALTIME_NULL,
    FC_LOG_FOPEN_FAILURE,
    FC_LOG_VSPRINTF_FAILURE,
    FC_LOG_SPRINTF_FAILURE,
    FC_LOG_FPUTS_FAILURE
};

enum FcLog_type_e {
    FC_LOG_DEBUG
};

typedef union {
    struct tm *(*pLocaltime)(const time_t *);

    struct tm *(*pLocaltime_r)(const time_t *, struct tm *);

    struct tm *(*pLocaltime_s)(const time_t *restrict, struct tm *restrict);


} FcLog_localtime_u;

typedef struct {
    const char *mFile_debug;
    const char *mFile_trace;
} FcLog_Setting_t;

typedef struct {
    FcLog_localtime_u mTime;

    FILE *mFile[1];
} FcLog_t;


typedef int32_t FcLog_Error_t;

FCDEF FcLog_Error_t
FcLog_cmd(uint32_t type, const char *pFile, uint32_t line, const char *pFormat, ...);



FCDEF FcLog_Error_t
FcLog_cmd(uint32_t type, char const* const pFile, uint32_t line, char const* const pFormat, ...) {
    va_list mList;
    FcLog_Error_t mError;
    int mLength, mOffset;
    time_t mTime;
    struct tm mTimeInfo;
    char mBuffer[LOG_BUFFER_SIZE];
    mError = FC_LOG_ERROR_INPUT;
    va_start(mList, pFormat);

#ifdef WIN32
    /*DWORD mMode = 0;
    HANDLE mHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (mHandle == INVALID_HANDLE_VALUE) {
        exit(GetLastError());
    }
    if(!GetConsoleMode(mHandle, &mMode)) {
    	exit(GetLastError());
    }
    if (!SetConsoleMode(mHandle, mMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        exit(GetLastError());
    }*/
#endif

    if (type > FC_LOG_DEBUG)
        goto gError;

    mError = !time(&mTime) * FC_LOG_TIME_NULL;
    if (mError)
        goto gError;
#ifdef _MSC_VER
    mError = localtime_s(&mTimeInfo, &mTime) * FC_LOG_LOCALTIME_NULL;
#elif defined __USE_POSIX
    mError = !localtime_r(&mTime, &mTimeInfo) * FC_LOG_LOCALTIME_NULL;
#else
#error
#endif
    if (mError)
        goto gError;
    mTimeInfo.tm_mon += 1;
    mTimeInfo.tm_year += 1900;
    mError = ((mOffset = sprintf(mBuffer, s_FcLog_format[1],
                                 mTimeInfo.tm_mday, mTimeInfo.tm_mon, mTimeInfo.tm_year, mTimeInfo.tm_hour,
                                 mTimeInfo.tm_min, mTimeInfo.tm_sec, pFile, line)) < 0) *
                                         FC_LOG_SPRINTF_FAILURE;
    if (mError)
        goto gError;

    mError = ((mLength = vsprintf(mBuffer + mOffset, pFormat, mList)) < 0) * FC_LOG_VSPRINTF_FAILURE;
    if (mError)
        goto gError;
    if (mLength + mOffset + 1 > LOG_BUFFER_SIZE) {
        puts("Memory overrun in FcLog_cmd() : LOG_BUFFER_SIZE");
        abort();
    }

    mError = (fputs(mBuffer, stdout) < 0) * FC_LOG_FPUTS_FAILURE;
    //mError = (fprintf(pLog->mFile[type], "%lu, %s", FcLog_MSR(), mBuffer) < 0) * FC_LOG_FPRINTF_FAILURE;
    if (mError)
        goto gError;


    //fflush(pLog->mFile[type]);
    gError:
    va_end(mList);
    return mError;
}
#endif //FCLOG_FCLOG_H
