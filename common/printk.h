#ifndef __KERNEL_PRINTK__
#define __KERNEL_PRINTK__

//#include <linux/init.h>
#include "kernel.h"
#include "kern_levels.h"

extern const char linux_banner[];
extern const char linux_proc_banner[];

static inline int printk_get_level(const char *buffer)
{
	if (buffer[0] == KERN_SOH_ASCII && buffer[1]) {
		switch (buffer[1]) {
		case '0' ... '7':
		case 'd':	/* KERN_DEFAULT */
			return buffer[1];
		}
	}
	return 0;
}

static inline const char *printk_skip_level(const char *buffer)
{
	if (printk_get_level(buffer))
		return buffer + 2;

	return buffer;
}

#define CONSOLE_EXT_LOG_MAX	8192

/* printk's without a loglevel use this.. */
#define MESSAGE_LOGLEVEL_DEFAULT CONFIG_MESSAGE_LOGLEVEL_DEFAULT

/* We show everything that is MORE important than this.. */
#define CONSOLE_LOGLEVEL_SILENT  0 /* Mum's the word */
#define CONSOLE_LOGLEVEL_MIN	 1 /* Minimum loglevel we let people use */
#define CONSOLE_LOGLEVEL_QUIET	 4 /* Shhh ..., when booted with "quiet" */
#define CONSOLE_LOGLEVEL_DEFAULT 7 /* anything MORE serious than KERN_DEBUG */
#define CONSOLE_LOGLEVEL_DEBUG	10 /* issue debug messages */
#define CONSOLE_LOGLEVEL_MOTORMOUTH 15	/* You can't shut this one up */

extern int console_printk[];

#define console_loglevel (console_printk[0])
#define default_message_loglevel (console_printk[1])
#define minimum_console_loglevel (console_printk[2])
#define default_console_loglevel (console_printk[3])

static inline void console_silent(void)
{
	console_loglevel = CONSOLE_LOGLEVEL_SILENT;
}

static inline void console_verbose(void)
{
	if (console_loglevel)
		console_loglevel = CONSOLE_LOGLEVEL_MOTORMOUTH;
}
/*
struct va_format {
	const char *fmt;
	va_list *va;
};
*/

/*
 * FW_BUG
 * Add this to a message where you are sure the firmware is buggy or behaves
 * really stupid or out of spec. Be aware that the responsible BIOS developer
 * should be able to fix this issue or at least get a concrete idea of the
 * problem by reading your message without the need of looking at the kernel
 * code.
 *
 * Use it for definite and high priority BIOS bugs.
 *
 * FW_WARN
 * Use it for not that clear (e.g. could the kernel messed up things already?)
 * and medium priority BIOS bugs.
 *
 * FW_INFO
 * Use this one if you want to tell the user or vendor about something
 * suspicious, but generally harmless related to the firmware.
 *
 * Use it for information or very low priority BIOS bugs.
 */
#define FW_BUG		"[Firmware Bug]: "
#define FW_WARN		"[Firmware Warn]: "
#define FW_INFO		"[Firmware Info]: "

/*
 * HW_ERR
 * Add this to a message for hardware errors, so that user can report
 * it to hardware vendor instead of LKML or software vendor.
 */
#define HW_ERR		"[Hardware Error]: "

/*
 * Dummy printk for disabled debugging statements to use whilst maintaining
 * gcc's format and side-effect checking.
 */
/*
static inline __attribute__ ((format (printf, 1, 2)))
int no_printk(const char *fmt, ...)
{
	return 0;
}

extern asmlinkage __attribute__ ((format (printf, 1, 2)))
void early_printk(const char *fmt, ...);

extern int printk_needs_cpu(int cpu);
extern void printk_tick(void);

#ifdef CONFIG_PRINTK
asmlinkage __attribute__ ((format (printf, 1, 0)))
int vprintk(const char *fmt, va_list args);
asmlinkage __attribute__ ((format (printf, 1, 2))) __cold
int printk(const char *fmt, ...);

#define printk_ratelimit() __printk_ratelimit(__func__)
extern bool printk_timed_ratelimit(unsigned long *caller_jiffies,
				   unsigned int interval_msec);

extern int printk_delay_msec;
extern int dmesg_restrict;
extern int kptr_restrict;

void log_buf_kexec_setup(void);
void __init setup_log_buf(int early);
#else
static inline __attribute__ ((format (printf, 1, 0)))
int vprintk(const char *s, va_list args)
{
	return 0;
}
static inline __attribute__ ((format (printf, 1, 2))) __cold
int printk(const char *s, ...)
{
	return 0;
}
static inline int printk_ratelimit(void)
{
	return 0;
}
static inline bool printk_timed_ratelimit(unsigned long *caller_jiffies,
					  unsigned int interval_msec)
{
	return false;
}

static inline void log_buf_kexec_setup(void)
{
}

static inline void setup_log_buf(int early)
{
}
#endif
*/
int printk(const char *s, ...);

//extern void dump_stack(void) __cold;

#ifndef pr_fmt
#include <errno.h>
#define pr_fmt(fmt) "%s:%d:%s(%d) " fmt, __FILE__, __LINE__, __FUNCTION__, errno
#endif

#define pr_emerg(fmt, ...) \
	printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...) \
	printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...) \
	printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...) \
	printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warning(fmt, ...) \
	printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn pr_warning
#define pr_notice(fmt, ...) \
	printk(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...) \
	printk(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#define pr_cont(fmt, ...) \
	printk(KERN_CONT fmt, ##__VA_ARGS__)

/* pr_devel() should produce zero code unless DEBUG is defined */
#ifdef DEBUG
#define pr_devel(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_devel(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif

/* If you are writing a driver, please use dev_dbg instead */
#if defined(DEBUG)
#define pr_debug(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#elif defined(CONFIG_DYNAMIC_DEBUG)
/* dynamic_pr_debug() uses pr_fmt() internally so we don't need it here */
#define pr_debug(fmt, ...) \
	dynamic_pr_debug(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif

/*
 * Print a one-time message (analogous to WARN_ONCE() et al):
 */

#ifdef CONFIG_PRINTK
#define printk_once(fmt, ...)			\
({						\
	static bool __print_once;		\
						\
	if (!__print_once) {			\
		__print_once = true;		\
		printk(fmt, ##__VA_ARGS__);	\
	}					\
})
#else
#define printk_once(fmt, ...)			\
	no_printk(fmt, ##__VA_ARGS__)
#endif

#define pr_emerg_once(fmt, ...)					\
	printk_once(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert_once(fmt, ...)					\
	printk_once(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit_once(fmt, ...)					\
	printk_once(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err_once(fmt, ...)					\
	printk_once(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn_once(fmt, ...)					\
	printk_once(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_notice_once(fmt, ...)				\
	printk_once(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info_once(fmt, ...)					\
	printk_once(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#define pr_cont_once(fmt, ...)					\
	printk_once(KERN_CONT pr_fmt(fmt), ##__VA_ARGS__)
/* If you are writing a driver, please use dev_dbg instead */
#if defined(DEBUG)
#define pr_debug_once(fmt, ...)					\
	printk_once(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_debug_once(fmt, ...)					\
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif

/*
 * ratelimited messages with local ratelimit_state,
 * no local ratelimit_state used in the !PRINTK case
 */
#ifdef CONFIG_PRINTK
#define printk_ratelimited(fmt, ...)					\
({									\
	static DEFINE_RATELIMIT_STATE(_rs,				\
				      DEFAULT_RATELIMIT_INTERVAL,	\
				      DEFAULT_RATELIMIT_BURST);		\
									\
	if (__ratelimit(&_rs))						\
		printk(fmt, ##__VA_ARGS__);				\
})
#else
#define printk_ratelimited(fmt, ...)					\
	no_printk(fmt, ##__VA_ARGS__)
#endif

#define pr_emerg_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_notice_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
/* no pr_cont_ratelimited, don't do that... */
/* If you are writing a driver, please use dev_dbg instead */
#if defined(DEBUG)
#define pr_debug_ratelimited(fmt, ...)					\
	printk_ratelimited(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_debug_ratelimited(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

extern int hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
			      int groupsize, char *linebuf, size_t linebuflen,
			      bool ascii);
#define CONFIG_PRINTK
#ifdef CONFIG_PRINTK
extern void print_hex_dump(const char *level, const char *prefix_str,
			   int prefix_type, int rowsize, int groupsize,
			   const void *buf, size_t len, bool ascii);
#if defined(CONFIG_DYNAMIC_DEBUG)
#define print_hex_dump_bytes(prefix_str, prefix_type, buf, len)	\
	dynamic_hex_dump(prefix_str, prefix_type, 16, 1, buf, len, true)
#else
extern void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
				 const void *buf, size_t len);
#endif /* defined(CONFIG_DYNAMIC_DEBUG) */
#else
static inline void print_hex_dump(const char *level, const char *prefix_str,
				  int prefix_type, int rowsize, int groupsize,
				  const void *buf, size_t len, bool ascii)
{
}
static inline void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
					const void *buf, size_t len)
{
}

#endif

#if defined(CONFIG_DYNAMIC_DEBUG)
#define print_hex_dump_debug(prefix_str, prefix_type, rowsize,	\
			     groupsize, buf, len, ascii)	\
	dynamic_hex_dump(prefix_str, prefix_type, rowsize,	\
			 groupsize, buf, len, ascii)
#else
#define print_hex_dump_debug(prefix_str, prefix_type, rowsize,		\
			     groupsize, buf, len, ascii)		\
	print_hex_dump(KERN_DEBUG, prefix_str, prefix_type, rowsize,	\
		       groupsize, buf, len, ascii)
#endif /* defined(CONFIG_DYNAMIC_DEBUG) */



void			tsnprintf_s(char *buffer, size_t count, const char *format, ...);
#define			Check(expr)			{ if(!(expr)) { printk(KERN_CRIT "%s:%d[%s][%s]",__FILE__, __LINE__, __FUNCTION__, #expr); } }
#define 		CheckEx(expr, msg)	{ if(!(expr)) { printk(KERN_CRIT "%s:%d[%s][%s][%s]",__FILE__, __LINE__, __FUNCTION__, #expr, msg); } }
#define			print_errno()	{ printk(KERN_ERR "%s:%d[%s],errno=%d:%s",__FILE__, __LINE__, __FUNCTION__,errno, strerror(errno)); }
#define			pr_errno()	{ printk(KERN_ERR "%s:%d[%s],errno=%d:%s",__FILE__, __LINE__, __FUNCTION__,errno, strerror(errno)); }

#endif
