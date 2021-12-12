#include "DialectTestUnit.hpp"

#include "TestDialect.hpp"

#include "matador/sql/sql.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/condition.hpp"

using namespace matador;

DialectTestUnit::DialectTestUnit()
  : unit_test("dialect", "dialect test unit")
{
  add_test("escaping", std::bind(&DialectTestUnit::test_escaping_quotes, this), "test dialect escaping");
  add_test("create", std::bind(&DialectTestUnit::test_create_query, this), "test create dialect");
  add_test("drop", std::bind(&DialectTestUnit::test_drop_query, this), "test drop dialect");
  add_test("insert", std::bind(&DialectTestUnit::test_insert_query, this), "test insert dialect");
  add_test("insert_prepare", std::bind(&DialectTestUnit::test_insert_prepare_query, this), "test prepared insert dialect");
  add_test("select_all", std::bind(&DialectTestUnit::test_select_all_query, this), "test select all dialect");
  add_test("select_distinct", std::bind(&DialectTestUnit::test_select_distinct_query, this), "test select distinct dialect");
  add_test("select_limit", std::bind(&DialectTestUnit::test_select_limit_query, this), "test select limit dialect");
  add_test("select_ordered", std::bind(&DialectTestUnit::test_select_ordered_query, this), "test select ordered dialect");
  add_test("select_grouped", std::bind(&DialectTestUnit::test_select_grouped_query, this), "test select grouped dialect");
  add_test("select_where", std::bind(&DialectTestUnit::test_select_where_query, this), "test select where dialect");
  add_test("update", std::bind(&DialectTestUnit::test_update_query, this), "test update dialect");
  add_test("update_where", std::bind(&DialectTestUnit::test_update_where_query, this), "test update where dialect");
  add_test("update_prepare", std::bind(&DialectTestUnit::test_update_prepare_query, this), "test prepared update dialect");
  add_test("update_where_prepare", std::bind(&DialectTestUnit::test_update_where_prepare_query, this), "test prepared update where dialect");
  add_test("delete", std::bind(&DialectTestUnit::test_delete_query, this), "test delete dialect");
  add_test("delete_where", std::bind(&DialectTestUnit::test_delete_where_query, this), "test delete where dialect");
}

void DialectTestUnit::test_escaping_quotes()
{
  TestDialect dialect;

  std::string str("baba\"gaga");

  dialect.escape_quotes_in_identifier(str);

  UNIT_ASSERT_EQUAL("baba\"\"gaga", str);
}

void DialectTestUnit::test_create_query()
{
  sql s;

  s.reset(t_query_command::CREATE);
  s.append(new detail::create("person"));

  std::unique_ptr<matador::columns> cols(new columns(columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<detail::typed_identifier_column>("id", data_type::type_unsigned_long, 0, false));
  cols->push_back(std::make_shared<detail::typed_varchar_column>("name", 256, data_type::type_varchar, 1, false));
  cols->push_back(std::make_shared<detail::typed_column>("age", data_type::type_unsigned_int, 2, false));

  s.append(cols.release());

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("create", s.command());
  UNIT_ASSERT_EQUAL("CREATE TABLE \"person\" (\"id\" INTEGER NOT NULL PRIMARY KEY, \"name\" VARCHAR(256), \"age\" INTEGER) ", result);
}

void DialectTestUnit::test_drop_query()
{
  sql s;

  s.reset(t_query_command::DROP);
  s.append(new detail::drop("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("drop", s.command());
  UNIT_ASSERT_EQUAL("DROP TABLE \"person\" ", result);
}

void DialectTestUnit::test_insert_query()
{
  sql s;

  s.reset(t_query_command::INSERT);
  s.table_name("person");
  s.append(new detail::insert("person"));

  std::unique_ptr<matador::columns> cols(new columns(columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  std::unique_ptr<matador::detail::values> vals(new detail::values);

  unsigned long id(8);
  std::string name("hans");
  unsigned int age(25);

  vals->push_back(std::make_shared<value>(id));
  vals->push_back(std::make_shared<value>(name));
  vals->push_back(std::make_shared<value>(age));

  s.append(vals.release());

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("person", s.table_name());
  UNIT_ASSERT_EQUAL("insert", s.command());
  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES (8, 'hans', 25) ", result);
}

void DialectTestUnit::test_insert_prepare_query()
{
  sql s;

  s.reset(t_query_command::INSERT);
  s.append(new detail::insert("person"));

  std::unique_ptr<matador::columns> cols(new columns(columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  std::unique_ptr<matador::detail::values> vals(new detail::values);

  unsigned long id(8);
  std::string name("hans");
  unsigned int age(25);

  vals->push_back(std::make_shared<value>(id));
  vals->push_back(std::make_shared<value>(name));
  vals->push_back(std::make_shared<value>(age));

  s.append(vals.release());

  TestDialect dialect;
  auto result = dialect.prepare(s);

  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES (?, ?, ?) ", std::get<0>(result));
}

void DialectTestUnit::test_select_all_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(new detail::select);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" ", result);
}

void DialectTestUnit::test_select_distinct_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(new detail::select);
  s.append(new detail::distinct);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT DISTINCT \"id\", \"name\", \"age\" FROM \"person\" ", result);
}

void DialectTestUnit::test_select_limit_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(new detail::select);
  s.append(new detail::top(10));

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT LIMIT 10 \"id\", \"name\", \"age\" FROM \"person\" ", result);
}

void DialectTestUnit::test_select_ordered_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(new detail::select);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));
  s.append(new detail::order_by("name"));
  s.append(new detail::desc);

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" ORDER BY \"name\" DESC ", result);
}

void DialectTestUnit::test_select_grouped_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(new detail::select);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));
  s.append(new detail::group_by("name"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" GROUP BY \"name\" ", result);
}

void DialectTestUnit::test_select_where_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(new detail::select);

  auto cols = std::make_unique<matador::columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  matador::column name("name");
  s.append(new detail::where(name != "hans"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE \"name\" <> 'hans' ", result);

  s.reset(t_query_command::SELECT);

  s.append(new detail::select);

  cols = std::make_unique<matador::columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  s.append(new detail::where(name != "Hans" && name != "Dieter"));

  result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE (\"name\" <> 'Hans' AND \"name\" <> 'Dieter') ", result);
}

void DialectTestUnit::test_update_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
  s.append(new detail::update);
  s.append(new detail::tablename("person"));
  s.append(new detail::set);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(std::make_shared<detail::value_column<std::string>>("name", dieter));
  cols->push_back(std::make_shared<detail::value_column<unsigned int>>("age", age54));

  s.append(cols.release());

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("update", s.command());
  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 ", result);
}

void DialectTestUnit::test_update_where_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
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
  matador::column age("age");
  s.append(new detail::where(name != "Hans" && matador::in(age, {7,5,5,8})));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("update", s.command());
  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 WHERE (\"name\" <> 'Hans' AND \"age\" IN (7,5,5,8)) ", result);
}

void DialectTestUnit::test_update_prepare_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
  s.append(new detail::update);
  s.append(new detail::tablename("person"));
  s.append(new detail::set);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(std::make_shared<detail::value_column<std::string>>("name", dieter));
  cols->push_back(std::make_shared<detail::value_column<unsigned int>>("age", age54));

  s.append(cols.release());

  TestDialect dialect;
  auto result = dialect.prepare(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"=?, \"age\"=? ", std::get<0>(result));
}

void DialectTestUnit::test_update_where_prepare_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
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
  matador::column age("age");
  s.append(new detail::where(name != "Hans" && matador::in(age, {7,5,5,8})));

  TestDialect dialect;
  auto result = dialect.prepare(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"=?, \"age\"=? WHERE (\"name\" <> ? AND \"age\" IN (?,?,?,?)) ", std::get<0>(result));

}

void DialectTestUnit::test_delete_query()
{
  sql s;

  s.reset(t_query_command::REMOVE);
  s.append(new detail::remove());
  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("delete", s.command());
  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" ", result);
}

void DialectTestUnit::test_delete_where_query()
{
  sql s;

  s.reset(t_query_command::REMOVE);
  s.append(new detail::remove());
  s.append(new detail::from("person"));

  matador::column name("name");
  matador::column age("age");
  s.append(new detail::where(name != "Hans" && matador::between(age, 21, 30)));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" WHERE (\"name\" <> 'Hans' AND \"age\" BETWEEN 21 AND 30) ", result);
}
