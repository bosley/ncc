/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

// Adapted from:
// https://github.com/rxi/log.c/tree/master

#ifndef LOG_HPP
#define LOG_HPP

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <functional>

#define LOG_VERSION "0.1.0"

namespace rxi {

typedef struct {
  va_list ap;
  const char *fmt{nullptr};
  const char *file{nullptr};
  struct tm *time{nullptr};
  void *udata{nullptr};
  int line{0};
  int level{0};
} log_Event;

using log_LogFn = std::function<void(log_Event *ev)>;
using log_LockFn = std::function<void(bool lock, void *udata)>;

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(rxi::LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(rxi::LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(rxi::LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(rxi::LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(rxi::LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(rxi::LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

const char *log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_log(int level, const char *file, int line, const char *fmt, ...);

} // namespace rxi

#endif
