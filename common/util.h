#ifndef _UTIL_H
#define _UTIL_H
#include <stdint.h>
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

#ifdef __cplusplus
#ifdef WIN32
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
#else
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
#endif
/*
void			print_u64(uint64_t value)
{
	//printf("%llu\n", value);
}
uint32_t			get_high_section(uint64_t uValue)
{
	return (uint32_t)((uValue >> 32) & 0xFFFFFFFF);
}

uint32_t			get_low_section(uint64_t uValue)
{
	return (uint32_t)(uValue & 0xFFFFFFFF);
}

uint64_t			touint64(uint32_t uHigh, uint32_t uLow)
{
	uint64_t u64 = uHigh;
	u64 = ((u64 << 32) | uLow);
	return u64;
}
*/
#endif
