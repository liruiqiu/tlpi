#include "Util.h"

int default_message_loglevel = 40;//DEFAULT_MESSAGE_LOGLEVEL;

void WriteLog(const char *cszFormat, ...)
{
	const char *p= cszFormat;
	int loglev;
	int printed_len;
	if (p[0] == '<' && p[1] >='0' &&
		p[1] <= '7' && p[2] == '>') {
			loglev = atoi(&p[1]);
			printed_len = 3;
	} else {
		loglev = default_message_loglevel + '0';
		printed_len = 0;
	} 

	//static int default_message_loglevel= DEFAULT_MESSAGE_LOGLEVEL;
	if (loglev >= default_message_loglevel) return;

	time_t	timeT;
	tm		curTime;

	time(&timeT);

#if defined(__WINDOWS__)
	tm	*ptm = localtime(&timeT);
	curTime = *ptm;
	unsigned int m_Now = GetTickCount();
#else
	tm	newtm;
	tm	*ptm = localtime_r(&timeT, &newtm);
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
	va_start(argptr, cszFormat);
#ifdef __WINDOWS__
	_vsnprintf(szBuf, 1023, cszFormat, argptr);
#else
	vsnprintf(szBuf, 1023, cszFormat, argptr);
#endif
	va_end(argptr);


	char szLogBuf[1024] = {0};
	sprintf( szLogBuf, "(%.2d:%.2d:%.2d.%d)<%d>[%d] %s\n", hour, minute, second, m_Now %1000, loglev, getpid(), &szBuf[printed_len]);

	//::OutputDebugStringA(szLogBuf);
	//printf("%s", szLogBuf);
	//return;

	char szFileName[260] = {0};
	if (loglev < DEFAULT_MESSAGE_LOGLEVEL)
	{
		sprintf( szFileName, "%s_%.4d-%.2d-%.2d.log", "./Log/Error", year, month, day );
	}
	else
	{
		sprintf( szFileName, "%s_%.4d-%.2d-%.2d.log", "./Log/Debug", year, month, day );
	}

	int nSize = (int)strlen( szLogBuf );
	lock_guard<FastLock> guard(channelLock);
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
	int	nchars = tvsnprintf((char *) buffer, count, format, argptr);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	va_end(argptr);

	if(nchars == count)
	{
		buffer[count - 1] = '\0';
	}

	if(nchars == -1)
	{
		//KCheck(FALSE);
		WriteLog(KERN_ERR "%s", buffer);
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
