#ifndef __EAVL_PLATFORM_H
#define __EAVL_PLATFORM_H

#ifdef _MSC_VER 
  #include <cstdarg>
  #include <stdio.h> 
  #include <stdarg.h>
  #include <varargs.h>
#endif

// -----------------------------------------------
// Variadic macros in MSVC are not the same as GCC
// -----------------------------------------------
#ifdef _MSC_VER 
  #define THROW(e , ...)                                                  \
  {                                                                           \
      e exception;                                                            \
      exception.SetTypeAndMessage(#e , ## __VA_ARGS__);                              \
      exception.SetThrowLocation(__FILE__, __LINE__, EAVL_CURRENT_FUNCTION);  \
      throw(exception);                                                       \
  }
#else
  #define THROW(e , args...)                                                  \
  {                                                                           \
      e exception;                                                            \
      exception.SetTypeAndMessage(#e , ## args);                              \
      exception.SetThrowLocation(__FILE__, __LINE__, EAVL_CURRENT_FUNCTION);  \
      throw(exception);                                                       \
  }
#endif

// -----------------------------------------------
// c99 defs : snprintf
// -----------------------------------------------
#ifdef _MSC_VER

#define snprintf c99_snprintf
#define strcasecmp _stricmp

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#endif // _MSC_VER
#endif
