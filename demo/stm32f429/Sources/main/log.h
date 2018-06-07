#ifndef __LOG_H
#define __LOG_H

void log_init(void);
void log_printf(const char *fmt, ...);

#define LOG   log_printf

#endif
