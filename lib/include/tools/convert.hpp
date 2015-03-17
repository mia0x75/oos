/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it wicll be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONVERT_HPP
#define CONVERT_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "tools/varchar.hpp"
#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/enable_if.hpp"

#include <type_traits>

#include <cstdlib>
#include <typeinfo>
#include <cstring>
#include <stdio.h>

/**
 * @file convert.hpp
 * @brief Provides generic conversion functions.
 * 
 * This file provides generic conversion function.
 * All builtin types, std::string and oos::varchar_base
 * types are supported. The conversion succeeds as long
 * as the size of the types fits or it is a string.
 */

namespace oos {

class date;
class time;

#ifdef OOS_DOXYGEN_DOC

/**
 * Convert a paramater of one type into another.
 * If the conversion is not allowed an exception
 * is thrown.
 * 
 * @tparam T Type of from parameter.
 * @tparam U Type of to parameter.
 * @param from From value to convert.
 * @param to Result parameter.
 * @throw std::bad_cast() exception.
 */
template < class T, class U >
void
convert(const T &from, U &to);

/**
 * Convert a paramater of one type into another.
 * If the conversion is not allowed an exception
 * is thrown.
 * 
 * @tparam T Type of from parameter.
 * @tparam U Type of to parameter.
 * @tparam S Size of to parameter.
 * @param from From value to convert.
 * @param to Result parameter.
 * @param size Size of the result parameter
 * @throw std::bad_cast() exception.
 */
template < class T, class U, class S >
void
convert(const T &from, U &to, S size);

/**
 * Convert a paramater of one type into another.
 * If the conversion is not allowed an exception
 * is thrown.
 * 
 * @tparam T Type of from parameter.
 * @tparam U Type of to parameter.
 * @tparam S Size of to parameter.
 * @tparam P Precision of to parameter.
 * @param from From value to convert.
 * @param to Result parameter.
 * @param size Size of the result parameter
 * @param precision Precision of the result parameter
 * @throw std::bad_cast() exception.
 */
template < class T, class U, class S, class P >
void
convert(const T &from, U &to, S size, P precision);

#else

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    integral U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<T, U>::value &&
                                !std::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::is_signed<T>::value &&
                                 std::is_signed<U>::value) ||
                                (std::is_unsigned<T>::value &&
                                 std::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    integral U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<T, U>::value &&
                                !std::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::is_signed<T>::value &&
                                 std::is_signed<U>::value) ||
                                (std::is_unsigned<T>::value &&
                                 std::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    signed
 * to
 *    unsigned
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<T, U>::value &&
                                !std::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::is_signed<T>::value &&
                                 std::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  if (from < 0) {
    throw std::bad_cast();
  } else {
    to = (U)from;
  }
}
/*******************************************
 * 
 * Convert from
 *    signed
 * to
 *    unsigned
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<T, U>::value &&
                                !std::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::is_signed<T>::value &&
                                 std::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   unsigned
 * to
 *   signed
 * where
 *   less
 */
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<T, U>::value &&
                                !std::is_same<U, bool>::value &&
                                !std::is_same<T, bool>::value>::type* = 0,
        typename oos::enable_if<(std::is_unsigned<T>::value &&
                                 std::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) < sizeof(U))>::type* = 0)
{
  to = from;
}
/*
 * from
 *   unsigned
 * to
 *   signed
 * where
 *   greater equal
 */
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<T, U>::value &&
                                !std::is_same<U, bool>::value &&
                                !std::is_same<T, bool>::value>::type* = 0,
        typename oos::enable_if<(std::is_unsigned<T>::value &&
                                 std::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) < sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    floating point U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_floating_point<T>::value &&
                                std::is_floating_point<U>::value &&
                                !std::is_same<T, U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
/*******************************************
 * 
 * Cannot convert from
 *    floating point T
 * to
 *    floating point U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_floating_point<T>::value &&
                                std::is_floating_point<U>::value &&
                                !std::is_same<T, U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    floating point U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_integral<T>::value &&
                                !std::is_same<T, bool>::value &&
                                std::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    floating point U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_integral<T>::value &&
                                !std::is_same<T, bool>::value &&
                                std::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    integral U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_floating_point<T>::value &&
                                std::is_integral<U>::value &&
                                !std::is_same<U, bool>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*****************
 * from
 *   bool
 * to
 *   arithmetic
 *
 *****************/
template < class T >
void
convert(bool from, T &to,
        typename oos::enable_if<std::is_integral<T>::value &&
                                !std::is_same<T, bool>::value>::type* = 0)
{
  to = (from ? 1 : 0);
}

template < class T >
void
convert(bool from, T &to,
        typename oos::enable_if<std::is_floating_point<T>::value>::type* = 0)
{
  to = (T)(from ? 1.0 : 0.0);
}
/*****************
 * from
 *   arithmetic
 * to
 *   bool
 *
 *****************/
template < class T >
void
convert(const T &from, bool &to,
        typename oos::enable_if<std::is_arithmetic<T>::value &&
                                !std::is_same<T, bool>::value>::type* = 0)
{
  to = from > 0;
}

/*****************
 * from
 *   bool
 * to
 *   bool
 *
 *****************/
OOS_API void convert(const bool &from, bool &to);

template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_arithmetic<T>::value &&
                                !std::is_same<T, bool>::value &&
                                !std::is_same<U, bool>::value &&
                                std::is_same<T, U>::value>::type* = 0)
{
  to = from;
}

/*
 * from
 *   bool
 * to
 *   char array
 */
template < class S >
void
convert(bool from, char *to, S size,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
#ifdef _MSC_VER
  _snprintf_s(to, size, size, (from ? "true" : "false"));
#else
  snprintf(to, size, (from ? "true" : "false"));
#endif
}

/*
 * from
 *   integral
 * to
 *   char array
 */
template < class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<std::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<!std::is_same<T, char>::value >::type* = 0,
        typename oos::enable_if<!std::is_same<T, unsigned char>::value >::type* = 0,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
#ifdef _MSC_VER
  _snprintf_s(to, size, size, "%d", from);
#else
  snprintf(to, size, "%lu", (long unsigned int)from);
#endif
}
/*
 * from
 *   integral
 * to
 *   char array
 */
template < class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<std::is_same<T, char>::value ||
                                std::is_same<T, unsigned char>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
#ifdef _MSC_VER
  _snprintf_s(to, size, size, "%c", from);
#else
  snprintf(to, size, "%c", from);
#endif
}
template < class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<std::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<!std::is_same<T, bool>::value >::type* = 0,
        typename oos::enable_if<!std::is_same<T, char>::value >::type* = 0,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
#ifdef _MSC_VER
  _snprintf_s(to, size, size, "%d", from);
#else
  snprintf(to, size, "%ld", (long unsigned int)from);
#endif
}

/*
 * from
 *   floating point
 * to
 *   char array
 */
template < class T, class S, class P >
void
convert(const T &from, char *to, S size, P precision,
        typename oos::enable_if<std::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<P>::value>::type* = 0)
{
  char buf[32];
#ifdef _MSC_VER
  _snprintf_s(buf, 32, 32, "%%0.%df", precision);
  _snprintf_s(to, size, size, buf, from);
#else
  snprintf(buf, 32, "%%0.%df", precision);
  snprintf(to, size, buf, from);
#endif
}

/*
 * from
 *   T
 * to
 *   char array
 */
template < class T, class S >
void
convert(const T &, char *, S ,
        typename oos::enable_if<!std::is_arithmetic<T>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < class S >
void
convert(const char *from, char *to, S size,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
  if (strlen(from) < (size_t)size) {
#ifdef _MSC_VER
    strcpy_s(to, size, from);
#else
    strcpy(to, from);
#endif
  } else {
    throw std::bad_cast();
  }
}

/*
 * from
 *   floating point
 * to
 *   string
 */
template < class T, class U, class P >
void
convert(const T &from, U &to, P precision,
        typename oos::enable_if<std::is_same<U, std::string>::value ||
                                std::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<std::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<P>::value>::type* = 0)
{
  char buf[256];
  convert(from, buf, 256, precision);
  to.assign(buf);
}

/*
 * from
 *   integral
 * to
 *   string/varchar
 */
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<std::is_same<U, std::string>::value ||
                                std::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<T>::value>::type* = 0)
{
  char buf[256];
  convert(from, buf, 256);
  to.assign(buf);
}
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<std::is_same<U, std::string>::value ||
                                std::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<std::is_floating_point<T>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   const char array
 * to
 *   signed integral
 */
template < class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<(std::is_integral<T>::value &&
                                 std::is_signed<T>::value)>::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtol(from, &ptr, 10));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const char array
 * to
 *   unsigned integral
 */
template < class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<(std::is_integral<T>::value &&
                                 std::is_unsigned<T>::value &&
                                 !std::is_same<T, bool>::value)>::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtoul(from, &ptr, 10));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const char array
 * to
 *   floating point
 */
template < class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<std::is_floating_point<T>::value>::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtod(from, &ptr));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const char array
 * to
 *   bool
 */
OOS_API void convert(const char *from, bool &to);
/*
 * from
 *   const string
 * to
 *   signed integral
 */
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<(std::is_arithmetic<U>::value &&
                                 (std::is_same<T, std::string>::value ||
                                  std::is_base_of<varchar_base, T>::value))>::type* = 0)
{
  convert(from.c_str(), to);
}

template < class T >
void
convert(const T&, char*,typename oos::enable_if<!std::is_same<T, oos::date>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < class T, class U, class S >
void
convert(const T &, U &, S ,
        typename oos::enable_if<!((std::is_same<U, std::string>::value ||
                                   std::is_base_of<varchar_base, U>::value) &&
                                  std::is_floating_point<T>::value)>::type* = 0,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < class T, class U, class S, class P >
void
convert(const T &, U &, S , P ,
        typename oos::enable_if<std::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<std::is_integral<P>::value>::type* = 0)
{
  throw std::bad_cast();
}

OOS_API void convert(const varchar_base &from, std::string &to);
OOS_API void convert(const std::string &from, varchar_base &to);
OOS_API void convert(const std::string &from, std::string &to);
OOS_API void convert(const varchar_base &from, varchar_base &to);

OOS_API void convert(const date &from, int &to);
OOS_API void convert(const date &from, long &to);
OOS_API void convert(const date &from, unsigned int &to);
OOS_API void convert(const date &from, unsigned long &to);

/*
 * From
 *  date
 * to
 *  T
 */
template < typename T >
void convert(const date &/*from*/, T &/*to*/)
{
  throw std::bad_cast();
}

/*
 * From
 *  time
 * to
 *  T
 */
template < typename T >
void convert(const oos::time &/*from*/, T &/*to*/)
{
  throw std::bad_cast();
}

OOS_API void convert(int from, date &to);
OOS_API void convert(unsigned int from, date &to);
OOS_API void convert(long from, date &to);
OOS_API void convert(unsigned long from, date &to);

/*
 * From
 *  T
 * to
 *  date
 */
template < typename T >
void convert(const T &/*from*/, date &/*to*/)
{
  throw std::bad_cast();
}

/*
 * From
 *  T
 * to
 *  time
 */
template < typename T >
void convert(const T &/*from*/, oos::time &/*to*/)
{
  throw std::bad_cast();
}

#endif /* OOS_DOXYGEN_DOC */

}

#endif /* CONVERT_HPP */
