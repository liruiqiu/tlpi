#ifndef _BASE_H
#define _BASE_H

#ifdef linux
#warning linux

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <libgen.h>

#include "atomic.h"
#include "base_define.h"
#include "base_enum.h"
#include "base.h"
#include "be_byteshift.h"
#include "bitops.h"
#include "compiler.h"
#include "endian.h"
#include "err.h"
#include "hash.h"
#include "kernel.h"
#include "kfifo.h"
#include "klist.h"
#include "le_byteshift.h"
#include "list.h"
#include "log2.h"
#include "poison.h"
#include "printk.h"
#include "rbtree_augmented.h"
#include "rbtree.h"
#include "ring_buffer.h"
#include "thread_pool.h"
#include "types.h"
#include "util.h"

#endif

#ifdef WIN32
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
#endif



#endif

