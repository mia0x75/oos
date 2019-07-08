//
// Created by sascha on 29.07.16.
//

#include "SQLiteDialectTestUnit.hpp"

#include "connections.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/dialect_token.hpp"

using namespace matador;

SQLiteDialectTestUnit::SQLiteDialectTestUnit()
  : unit_test("sqlite_dialect", "sqlite dialect test")
{
  add_test("update_limit", std::bind(&SQLiteDialectTestUnit::test_update_with_limit, this), "test sqlite update limit compile");
  add_test("delete_limit", std::bind(&SQLiteDialectTestUnit::test_delete_with_limit, this), "test sqlite delete limit compile");
}

void SQLiteDialectTestUnit::test_update_with_limit()
{
  matador::connection conn(::connection::sqlite);

  sql s;

  s.append(new detail::update);
  s.append(new detail::tablename("person"));
  s.append(new detail::set);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(std::make_shared<detail::value_column<std::string>>("name", dieter));
  cols->push_back(std::make_shared<detail::value_column<unsigned int>>("age", age54));

  s.append(cols.release());

  matador::column name("name");
  s.append(new detail::where(name != "Hans"));

  s.append(new detail::top(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 WHERE \"rowid\" IN (SELECT \"rowid\" FROM \"person\" WHERE \"name\" <> 'Hans' LIMIT 1 ) ", result);
}

void SQLiteDialectTestUnit::test_delete_with_limit()
{
  matador::connection conn(::connection::sqlite);

  sql s;

  s.append(new detail::remove());
  s.append(new detail::from("person"));

  matador::column name("name");
  s.append(new detail::where(name != "Hans"));

  s.append(new detail::top(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" WHERE \"rowid\" IN (SELECT \"rowid\" FROM \"person\" WHERE \"name\" <> 'Hans' LIMIT 1 ) ", result);
}
