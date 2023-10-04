/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "matador/db/mssql/mssql_exception.hpp"

#include "matador/sql/database_error.hpp"

#include <sql.h>

#include <string>
#include <sstream>

namespace matador {

namespace mssql {

std::string error_message(const std::string &source, const std::string &sql)
{
  std::stringstream msg;
  msg << source << ": " << " (" << sql << ")";
  return msg.str();
}

void throw_database_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &sql)
{
  if (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA) {
    return;
  }
  SQLCHAR state[6];
  SQLINTEGER error;
  SQLCHAR data[512];
  SQLSMALLINT over_by;

  ret = SQL_ERROR;
  SQLSMALLINT i = 0;
  ret = SQLGetDiagRec(htype, hndl, ++i, state, &error, data, 511, &over_by);
  if (ret == SQL_SUCCESS) {
    std::string what(reinterpret_cast<char*>(data));
    std::string sqlstate(reinterpret_cast<char*>(state));
    throw database_error(what.c_str(), source, sqlstate, error, sql);
  }
}

//void throw_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &msg)
//{
//  if (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA) {
//    return;
//  }
//  SQLCHAR state[6];
//  SQLINTEGER error;
//  SQLCHAR data[512];
//  SQLSMALLINT over_by;
//
//  ret = SQL_ERROR;
//  SQLSMALLINT i = 0;
//  std::stringstream text;
//  do {
//    ret = SQLGetDiagRec(htype, hndl, ++i, state, &error, data, 511, &over_by);
//    if (ret == SQL_SUCCESS) {
//      text << "odbc error [" << state << "] " << msg << " " << i << " (" << error << "): " << data;
//    }
//  } while (ret == SQL_SUCCESS);
//
//  throw mssql_exception(source, text.str());
//}

bool is_success(SQLRETURN ret)
{
  return ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO;
}

}

}
