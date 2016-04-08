#ifndef SMT_LOG_H
#define SMT_LOG_H

#ifdef SMT_DEBUG
#define smtDbgs(s) puts(s)
#define smtDbgf(f,...) printf(f,##__VA_ARGS__)
#else
#define smtDbgs(s) ((void)0)
#define smtDbgf(f,...) ((void)0)
#endif

#endif
