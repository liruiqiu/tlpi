#ifndef _UTIL_H
#define _UTIL_H

#define __LINUX__

#if defined(__WINDOWS__)
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)

// warning C4819: The file contains a character that cannot be represented in the current code page (936).
#pragma warning(disable:4819)
#pragma warning(disable:4996)
#pragma warning(disable:4995)

#include <Windows.h>
#include <crtdbg.h>
#include <process.h>
#include <io.h>
#elif defined(__LINUX__)
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
//#include <iostream>
//#include <fstream>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#endif


//using namespace std;
#define IP_SIZE					(24)
#define INVALID_HANDLE			(-1)
#define INVALID_ID				(-1)
#define INVALID_SOCKET			(-1)
#ifndef SOCKET_ERROR
#define SOCKET_ERROR				(-1)
#endif

#define _NET_TRY try
#define  _NET_CATCH catch(...)

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_FREE
#define SAFE_FREE(p)      { if((p) != NULL) { free(p); (p)=NULL; } }
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define KERN_EMERG  "<0>"   /* system is unusable       紧急情况    */
#define KERN_ALERT  "<1>"   /* action must be taken immediately 需要立即被注意的错误*/
#define KERN_CRIT   "<2>"   /* critical conditions          临界情况*/
#define KERN_ERR    "<3>"   /* error conditions         错误*/
#define KERN_WARNING    "<4>"   /* warning conditions           警告*/
#define KERN_NOTICE "<5>"   /* normal but significant condition 普通的，不过也有可能需要注意的情况*/
#define KERN_INFO   "<6>"   /* informational            非正式的消息*/
#define KERN_DEBUG  "<7>"   /* debug-level messages         调试信息--一般冗余信息*/
#define DEFAULT_MESSAGE_LOGLEVEL (4)				/* 日志默认级别 */

#if defined(__WINDOWS__)
#define tvsnprintf	_vsnprintf
#define tstricmp	_stricmp
#define tsnprintf	_snprintf
#elif defined(__LINUX__)
#define tvsnprintf	vsnprintf
#define tstricmp	strcasecmp
#define tsnprintf	snprintf
#endif

/*
   February 2010 - changed list format to have a prev pointer from the
   list head. This makes DLIST_ADD_END() O(1) even though we only have
   one list pointer.

   The scheme is as follows:

   1) with no entries in the list:
   list_head == NULL

   2) with 1 entry in the list:
   list_head->next == NULL
   list_head->prev == list_head

   3) with 2 entries in the list:
   list_head->next == element2
   list_head->prev == element2
   element2->prev == list_head
   element2->next == NULL

   4) with N entries in the list:
   list_head->next == element2
   list_head->prev == elementN
   elementN->prev == element{N-1}
   elementN->next == NULL

   This allows us to find the tail of the list by using
   list_head->prev, which means we can add to the end of the list in
   O(1) time


   Note that the 'type' arguments below are no longer needed, but
   are kept for now to prevent an incompatible argument change
   */


/*
   add an element at the front of a list
   */
#define DLIST_ADD(list, p) \
	do { \
		if (!(list)) { \
			(p)->prev = (list) = (p);  \
			(p)->next = NULL; \
		} else { \
			(p)->prev = (list)->prev; \
			(list)->prev = (p); \
			(p)->next = (list); \
			(list) = (p); \
		} \
	} while (0)

/*
   remove an element from a list
   Note that the element doesn't have to be in the list. If it
   isn't then this is a no-op
   */
#define DLIST_REMOVE(list, p) \
	do { \
		if ((p) == (list)) { \
			if ((p)->next) (p)->next->prev = (p)->prev; \
			(list) = (p)->next; \
		} else if ((list) && (p) == (list)->prev) {	\
			(p)->prev->next = NULL; \
			(list)->prev = (p)->prev; \
		} else { \
			if ((p)->prev) (p)->prev->next = (p)->next; \
			if ((p)->next) (p)->next->prev = (p)->prev; \
		} \
		if ((p) != (list)) (p)->next = (p)->prev = NULL;	\
	} while (0)

/*
   find the head of the list given any element in it.
   Note that this costs O(N), so you should avoid this macro
   if at all possible!
   */
#define DLIST_HEAD(p, result_head) \
	do { \
		(result_head) = (p); \
		while (DLIST_PREV(result_head)) (result_head) = (result_head)->prev; \
	} while(0)

/* return the last element in the list */
#define DLIST_TAIL(list) ((list)?(list)->prev:NULL)

/* return the previous element in the list. */
#define DLIST_PREV(p) (((p)->prev && (p)->prev->next != NULL)?(p)->prev:NULL)

/* insert 'p' after the given element 'el' in a list. If el is NULL then
   this is the same as a DLIST_ADD() */
#define DLIST_ADD_AFTER(list, p, el) \
	do { \
		if (!(list) || !(el)) { \
			DLIST_ADD(list, p); \
		} else { \
			(p)->prev = (el);   \
			(p)->next = (el)->next;		\
			(el)->next = (p);		\
			if ((p)->next) (p)->next->prev = (p);	\
			if ((list)->prev == (el)) (list)->prev = (p); \
		}\
	} while (0)


/*
   add to the end of a list.
   Note that 'type' is ignored
   */
#define DLIST_ADD_END(list, p, type)			\
	do { \
		if (!(list)) { \
			DLIST_ADD(list, p); \
		} else { \
			DLIST_ADD_AFTER(list, p, (list)->prev); \
		} \
	} while (0)

/* promote an element to the from of a list */
#define DLIST_PROMOTE(list, p) \
	do { \
		DLIST_REMOVE(list, p); \
		DLIST_ADD(list, p); \
	} while (0)

/*
   demote an element to the end of a list.
   Note that 'type' is ignored
   */
#define DLIST_DEMOTE(list, p, type)			\
	do { \
		DLIST_REMOVE(list, p); \
		DLIST_ADD_END(list, p, NULL);		\
	} while (0)

/*
   concatenate two lists - putting all elements of the 2nd list at the
   end of the first list.
   Note that 'type' is ignored
   */
#define DLIST_CONCATENATE(list1, list2, type)	\
	do { \
		if (!(list1)) { \
			(list1) = (list2); \
		} else { \
			(list1)->prev->next = (list2); \
			if (list2) { \
				void *_tmplist = (void *)(list1)->prev; \
				(list1)->prev = (list2)->prev; \
				(list2)->prev = _tmplist; \
			} \
		} \
	} while (0)

#if defined(__WINDOWS__)
class FastLock
{
	CRITICAL_SECTION m_Lock;
	bool m_Status;
	public:
	FastLock()
	{
		InitializeCriticalSection(&m_Lock);
		m_Status = false;
	}
	~FastLock()
	{
		DeleteCriticalSection(&m_Lock);
		m_Status = true;
	}
	void Lock()
	{
		EnterCriticalSection(&m_Lock);
		m_Status = true;
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_Lock);
		m_Status = false;
	}
};
#elif defined(__LINUX__)
class	FastLock
{
	pthread_mutex_t m_Mutex;
	public:
	bool	mStatus;
	FastLock()
	{
		pthread_mutex_init(&m_Mutex, NULL);
		mStatus = false;
	};
	~FastLock()
	{
		pthread_mutex_destroy(&m_Mutex);
	};
	void Lock()
	{
		pthread_mutex_lock(&m_Mutex);
		mStatus = true;
	};
	void Unlock()
	{
		pthread_mutex_unlock(&m_Mutex);
		mStatus = false;
	};
};
#endif


template<typename Mutex>
class lock_guard
{
	private:
		Mutex			&m;
		explicit		lock_guard(lock_guard &);
		lock_guard &operator	=(lock_guard &);

	public:
		explicit lock_guard(Mutex &m_) :
			m(m_)
	{
		m.Lock();
	}
		~lock_guard(void)
		{
			m.Unlock();
		}
};

#if defined(__WINDOWS__)
typedef unsigned long			TID_t;
#elif defined(__LINUX__)
typedef pthread_t					TID_t;
#endif

typedef short					ID_t;
typedef ID_t					ConnectorID_t;

typedef unsigned short		MsgID_t;

typedef int						PID_t;
typedef unsigned int			IP_t;

static FastLock channelLock;
extern void 	WriteLog(const char *cszFormat, ...);
void			tsnprintf_s(char *buffer, size_t count, const char *format, ...);
void			PrintU64(uint64_t value);
uint32_t		GetHighSection(uint64_t uValue);			
uint32_t		GetLowSection(uint64_t uValue);
uint64_t		ToUint64(uint32_t uHigh, uint32_t uLow);

#define			Check(expr)			{ if(!(expr)) { WriteLog(KERN_CRIT "%s:%d[%s][%s]",__FILE__, __LINE__, __FUNCTION__, #expr); } }
#define 		CheckEx(expr, msg)	{ if(!(expr)) { WriteLog(KERN_CRIT "%s:%d[%s][%s][%s]",__FILE__, __LINE__, __FUNCTION__, #expr, msg); } }
#define			print_errno()	{ WriteLog(KERN_ERR "%s:%d[%s],errno=%d:%s",__FILE__, __LINE__, __FUNCTION__,errno, strerror(errno)); }

#define MAX_URL  (1024)
#define MAX_COOKIE  (4096)
#define GMT_2_LOCAL_TIME_DIFF	(8*3600)

/********************************** linux and windows  end *******************************************************/

/************************************************************************/
/*						windows                                         */
/************************************************************************/
#if defined(__WINDOWS__)
#define GUID_INIT(x)						{ memset(&x,0,sizeof(x));}
#endif

/************************************************************************/
/*						linux 											*/
/************************************************************************/
extern int default_message_loglevel;
#endif
