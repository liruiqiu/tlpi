#ifndef _BASE_DEFINE_H
#define _BASE_DEFINE_H
/*
# define unlikely(cond)  __glibc_unlikely(cond)
# define likely(cond)  __glibc_likely(cond)
#define __must_check    __attribute__((warn_unused_result))     
*/


//using namespace std;
#define IP_SIZE					(24)
#define INVALID_ID				(-1)
#define INVALID_FD			    (-1)
#ifndef SOCKET_ERROR
#define SOCKET_ERROR			(-1)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_FREE
#define SAFE_FREE(p)      { if((p) != NULL) { free(p); (p)=NULL; } }
#endif

#define MAX_URL     (1024)
#define MAX_COOKIE  (4096)
#define GMT_2_LOCAL_TIME_DIFF	(8*3600)

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifdef __cplusplus
#define _NET_TRY try
#define  _NET_CATCH catch(...)
#endif

#if defined(WIN32)
#define tvsnprintf	_vsnprintf
#define tstricmp	_stricmp
#define tsnprintf	_snprintf
#elif defined(linux)
#define tvsnprintf	vsnprintf
#define tstricmp	strcasecmp
#define tsnprintf	snprintf
#endif


#endif
