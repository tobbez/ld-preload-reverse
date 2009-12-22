/*
 * Copyright 2009, Torbjörn Lönnemark <tobbez@ryara.net>
 * Licensed under the Eiffel Forum License 2.
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdarg.h>

#if defined(RTLD_NEXT)
#define REAL_LIBC RTLD_NEXT
#else
#define REAL_LIBC ((void *) -1L)
#endif


/*
 * reverse the string that starts at `start` and ends at `end`
 */
static void revstr(char *start, char *end)
{
  int i;

  if(start == end) return;

  for(i = 0; i < (end - start)/2 + 1; ++i) {
    char tmp;
    tmp = *(start + i);
    *(start + i) = *(end - i);
    *(end - i) = tmp;
  }
}

/*
 * in-place reverse of the string that starts at `src`
 */
static void revstrml(char *src)
{
  char *cur, *prev;

  cur = strchr(src, '\n');
  prev = src;

  while (cur != NULL) {
    if(cur != prev)
      revstr(prev, cur - 1);
    prev = ++cur;
    cur = strchr(cur, '\n');
  }

  if(*(src + strlen(src) - 1) != '\n')
    revstr(prev, src + strlen(src) - 1);
}


/*
 * overridden functions
 */

ssize_t write(int fd, const void *buf, size_t count)
{
  size_t retval;
  static size_t (*o_write) (int fd, const void *buf, size_t count) = 0;
  o_write = (size_t(*)(int fd, const void *buf, size_t count)) dlsym(REAL_LIBC, "write");

  if (fd == 1 || fd == 2) {
    char *tmp;
    tmp = malloc(strlen(buf) + 1);
    strcpy(tmp, (char*)buf);
    revstrml(tmp);

    retval = (*o_write)(fd, (void*)tmp, count);

    free(tmp);
  } else {
    retval = (*o_write)(fd, buf, count);
  }
  return retval;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  char *tmp;
  size_t retval = 0;

  static size_t (*o_fwrite) (const void *ptr, size_t size, size_t nmemb, FILE *stream) = 0;
  o_fwrite = (size_t(*)(const void *ptr, size_t size, size_t nmemb, FILE *stream)) dlsym(REAL_LIBC, "fwrite");

  if (size == 1 && (stream == stdout || stream == stderr)) {
    tmp = malloc(nmemb + 1);
    strcpy(tmp, (char*)ptr);

    revstrml(tmp);

    retval = (*o_fwrite)((void*) tmp, size, nmemb, stream);

    free(tmp);
    return retval;
  }

  retval = (*o_fwrite)(ptr, size, nmemb, stream);

  return retval;
}

int puts (const char *s)
{
  char *tmp;
  int retval;

  static int (*o_puts) (const char *s) = 0;
  o_puts = (int(*)(const char *s)) dlsym(REAL_LIBC, "puts");

  tmp = malloc(strlen(s) + 1);

  strcpy(tmp, s);
  revstrml(tmp);

  retval = (*o_puts)(tmp);

  free(tmp);

  return retval;
}

int fputs(const char *s, FILE *stream)
{
  int retval;
  static int (*o_fputs) (const char *s, FILE *stream) = 0;  
  o_fputs = (int (*) (const char *s, FILE *stream)) dlsym(REAL_LIBC, "fputs");

  if(stream == stdout || stream == stderr) {
    char *tmp;
    tmp = malloc(strlen(s) + 1);
    strcpy(tmp, s);
    revstrml(tmp);

    retval = (*o_fputs)(tmp, stream);

    free(tmp);    
  } else {
    retval = (*o_fputs)(s, stream);
  }
  return retval;
}

int printf(const char *format, ...)
{
  int retval;
  char *tmp;
  va_list ap;

  size_t slen;

  static int (*o_printf) (const char *format, ...) = 0;
  o_printf = (int (*) (const char *format, ...)) dlsym(REAL_LIBC, "printf");

  va_start(ap, format);  

  slen = vsnprintf(NULL, 0, format, ap);
  tmp = malloc(slen + 1);

  va_start(ap, format);
  vsnprintf(tmp, slen + 1, format, ap);

  revstrml(tmp);
  retval = (*o_printf)("%s", tmp);
  free(tmp);

  return retval;
}

int fprintf(FILE *stream, const char *format, ...)
{
  int retval;
  if(stream == stdout || stream == stderr) {
    va_list ap;
    char *tmp;

    size_t slen;

    static int (*o_fprintf) (FILE *stream, const char *format, ...) = 0;
    o_fprintf = (int (*) (FILE *stream, const char *format, ...)) dlsym(REAL_LIBC, "fprintf");

    va_start(ap, format);

    slen = vsnprintf(NULL, 0, format, ap);
    tmp = malloc(slen + 1);

    va_start(ap, format);
    vsnprintf(tmp, slen + 1, format, ap);

    revstrml(tmp);

    retval = (*o_fprintf)(stream, "%s", tmp);

    free(tmp);
  } else {
    va_list ap;
    va_start(ap, format);

    retval = vfprintf(stream, format, ap);
  }

  return retval;
}

size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream)
{
  char *tmp;
  size_t retval = 0;

  static size_t (*o_fwrite_unlocked) (const void *ptr, size_t size, size_t n, FILE *stream) = 0;
  o_fwrite_unlocked = (size_t(*)(const void *ptr, size_t size, size_t n, FILE *stream)) dlsym(REAL_LIBC, "fwrite_unlocked");

  if (size == 1 && (stream == stdout || stream == stderr)) {
    tmp = malloc(n + 1);
    strcpy(tmp, (char*)ptr);

    revstrml(tmp);

    retval = (*o_fwrite_unlocked)((void*) tmp, size, n, stream);

    free(tmp);
    return retval;
  }

  retval = (*o_fwrite_unlocked)(ptr, size, n, stream);

  return retval;  
}

