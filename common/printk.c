#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "printk.h"

/* printk's without a loglevel use this.. */
#define DEFAULT_MESSAGE_LOGLEVEL 40

/* We show everything that is MORE important than this.. */
#define MINIMUM_CONSOLE_LOGLEVEL 1 /* Minimum loglevel we let people use */
#define DEFAULT_CONSOLE_LOGLEVEL 7 /* anything MORE serious than KERN_DEBUG */

//DECLARE_WAIT_QUEUE_HEAD(log_wait);

int console_printk[4] = {
    DEFAULT_CONSOLE_LOGLEVEL,/* console_loglevel */
    DEFAULT_MESSAGE_LOGLEVEL,/* default_message_loglevel */
    MINIMUM_CONSOLE_LOGLEVEL,/* minimu      m_console_loglevel */
    DEFAULT_CONSOLE_LOGLEVEL,/* default_console_log level */
};

int printk(const char *s, ...)
{
    const char *p= s;
    int loglev;
    int printed_len;
    if (p[0] == '<' && p[1] >='0' &&
            p[1] <= '7' && p[2] == '>') {
        loglev = atoi(&p[1]);
        printed_len = 3;
    } else {
        loglev = default_message_loglevel;
        printed_len = 0;
    } 

    if (loglev > default_message_loglevel) return;

    time_t	timeT;
    struct tm		curTime;

    time(&timeT);

#if defined(__WINDOWS__)
    tm	*ptm = localtime(&timeT);
    curTime = *ptm;
    unsigned int m_Now = GetTickCount();
#else
    struct tm	newtm;
    struct tm	*ptm = localtime_r(&timeT, &newtm);
    if(ptm) 
        curTime = newtm;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int m_Now = tv.tv_usec;
#endif

    int year = curTime.tm_year + 1900;
    int month = curTime.tm_mon + 1;
    int day = curTime.tm_mday;
    int hour = curTime.tm_hour;
    int minute = curTime.tm_min;
    int second = curTime.tm_sec;


    char szBuf[1024] = {0};

    va_list argptr;
    va_start(argptr, s);
#ifdef __WINDOWS__
    _vsnprintf(szBuf, 1023, s, argptr);
#else
    vsnprintf(szBuf, 1023, s, argptr);
#endif
    va_end(argptr);


    char szLogBuf[1024] = {0};
    sprintf( szLogBuf, "(%.2d:%.2d:%.2d.%.3d)<%d>%s\n", hour, minute, second, m_Now %1000, loglev, &szBuf[printed_len]);

    //::OutputDebugStringA(szLogBuf);
    printf("%s", szLogBuf);
    //return;

    char szFileName[260] = {0};
    //if (loglev < DEFAULT_MESSAGE_LOGLEVEL)
    //	sprintf( szFileName, "%s_%.4d-%.2d-%.2d.log", "./Log/Error", year, month, day );
    //else
    sprintf( szFileName, "%s_%.4d-%.2d-%.2d.log", "./Log/Debug", year, month, day );

    int nSize = (int)strlen( szLogBuf );
    //lock_guard<FastLock> guard(channelLock);
    FILE* file = fopen( szFileName, "a+" );
    if( file != NULL )
    {
        fwrite( szLogBuf, 1, nSize, file );
        fclose( file );
    }
}

void tsnprintf_s(char *buffer, size_t count, const char *format, ...)
{
    /*~~~~~~~~~~~*/
    va_list argptr;
    /*~~~~~~~~~~~*/

    va_start(argptr, format);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int	nchars = vsnprintf((char *) buffer, count, format, argptr);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    va_end(argptr);

    if(nchars == count)
    {
        buffer[count - 1] = '\0';
    }

    if(nchars == -1)
    {
        //KCheck(FALSE);
        printk(KERN_ERR "%s", buffer);
    }
}

void PrintU64(uint64_t value)
{
    printf("%llu\n", value);
}

uint32_t			GetHighSection(uint64_t uValue)
{
    return (uint32_t)((uValue >> 32) & 0xFFFFFFFF);
}

uint32_t			GetLowSection(uint64_t uValue)
{
    return (uint32_t)(uValue & 0xFFFFFFFF);
}

uint64_t			ToUint64(uint32_t uHigh, uint32_t uLow)
{
    uint64_t u64 = uHigh;
    u64 = ((u64 << 32) | uLow);
    return u64;
}
