//
// Created by sascha on 3/9/16.
//

#include "matador/db/mssql/mssql_dialect.hpp"
#include "matador/db/mssql/mssql_dialect_compiler.hpp"
#include "matador/db/mssql/mssql_dialect_linker.hpp"

#include <algorithm>
#include <cstring>

namespace matador {

namespace mssql {

mssql_dialect::mssql_dialect()
  : basic_dialect(new mssql_dialect_compiler, new mssql_dialect_linker)
{
  replace_token(detail::token::BEGIN, "BEGIN TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT");
  replace_token(detail::token::ROLLBACK, "ROLLBACK");
  replace_token(detail::token::TOP, "TOP");
  replace_token(detail::token::START_QUOTE, "[");
  replace_token(detail::token::END_QUOTE, "]");
}

const char* mssql_dialect::to_database_type_string(matador::data_type type) const
{
  switch(type) {
    case data_type::type_char:
      return "SMALLINT";
    case data_type::type_short:
      return "SMALLINT";
    case data_type::type_int:
      return "INT";
    case data_type::type_long:
      return "BIGINT";
    case data_type::type_long_long:
      return "BIGINT";
    case data_type::type_unsigned_char:
      return "SMALLINT";
    case data_type::type_unsigned_short:
      return "INT";
    case data_type::type_unsigned_int:
      return "BIGINT";
    case data_type::type_unsigned_long:
      return "NUMERIC(21,0)";
    case data_type::type_unsigned_long_long:
      return "NUMERIC(21,0)";
    case data_type::type_bool:
      return "BIT";
    case data_type::type_float:
      return "FLOAT(24)";
    case data_type::type_double:
      return "FLOAT(53)";
    case data_type::type_char_pointer:
      return "VARCHAR";
    case data_type::type_varchar:
      return "VARCHAR";
    case data_type::type_text:
      return "VARCHAR(MAX)";
    case data_type::type_date:
      return "DATE";
    case data_type::type_time:
      return "DATETIME";
    default:
    {
      std::stringstream msg;
      msg << "mssql connection: unknown type [" << (int)type << "]";
      throw std::logic_error(msg.str());
    }
  }
}

database_type mssql_dialect::string_type(const char *type) const
{
  if (strcmp(type, "numeric") == 0) {
    return database_type::type_bigint;
  } else if (strcmp(type, "bigint") == 0) {
    return database_type::type_bigint;
  } else if (strcmp(type, "smallint") == 0) {
    return database_type::type_smallint;
  } else if (strcmp(type, "tinyint") == 0) {
    return database_type::type_char;
  } else if (strcmp(type, "int") == 0) {
    return database_type::type_int;
  } else if (strcmp(type, "bit") == 0) {
    return database_type::type_bool;
  } else if (strcmp(type, "date") == 0) {
    return database_type::type_date;
  } else if (strcmp(type, "datetime") == 0) {
    return database_type::type_time;
  } else if (strcmp(type, "float(24)") == 0) {
    return database_type::type_float;
  } else if (strcmp(type, "float(53)") == 0) {
    return database_type::type_double;
  } else if (strcmp(type, "real") == 0) {
    return database_type::type_double;
  } else if (strcmp(type, "varchar(max)") == 0) {
    return database_type::type_text;
  } else if (strcmp(type, "varchar") == 0) {
    return database_type::type_varchar;
  } else {
    return database_type::type_unknown;
  }
}

dialect_traits::identifier mssql_dialect::identifier_escape_type() const
{
  return dialect_traits::ESCAPE_CLOSING_BRACKET;
}

}

}
