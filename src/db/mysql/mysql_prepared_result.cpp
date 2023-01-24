#include "matador/db/mysql/mysql_prepared_result.hpp"
#include "matador/db/mysql/mysql_statement.hpp"
#include "matador/db/mysql/mysql_exception.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include <cstring>

namespace matador {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(mysql_statement *owner, MYSQL_STMT *s, std::vector<MYSQL_BIND> &bind, std::vector<mysql_result_info> &info)
  : affected_rows_((size_type)mysql_stmt_affected_rows(s))
  , rows((size_type)mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , owner_(owner)
  , stmt(s)
  , bind_(bind)
  , info_(info)
{
}

mysql_prepared_result::~mysql_prepared_result()
{
  free();
}

void mysql_prepared_result::free()
{
  if (stmt != nullptr) {
    mysql_stmt_free_result(stmt);
    stmt = nullptr;
  }

  if (owner_ != nullptr) {
    owner_->unlink_result(this);
    owner_ = nullptr;
  }
}

const char* mysql_prepared_result::column(size_type ) const
{
  return "not implemented";
}

bool mysql_prepared_result::fetch()
{
  // get next row
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_DATA_TRUNCATED) {
    // Todo: handle truncated data
  }
  return rows-- > 0;
}

bool mysql_prepared_result::prepare_fetch()
{
  // reset result column index
  result_index_ = 0;
  // fetch data
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == 1) {
    throw_stmt_error(stmt, "mysql", "");
  }
  prepare_binding_ = false;
  return true;
}

bool mysql_prepared_result::finalize_fetch()
{
  return true;
}

mysql_prepared_result::size_type mysql_prepared_result::affected_rows() const
{
  std::size_t ar = mysql_stmt_affected_rows(stmt);
  return (unsigned long)ar;
  //return affected_rows_;
}

mysql_prepared_result::size_type mysql_prepared_result::result_rows() const
{
  return rows;
}

mysql_prepared_result::size_type mysql_prepared_result::fields() const
{
  return fields_;
}

int mysql_prepared_result::transform_index(int index) const
{
  return index;
}

void mysql_prepared_result::read_value(const char *id, int index, int row, char &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_TINY, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, short &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_SHORT, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, int &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONG, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, long long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, unsigned char &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_VAR_STRING, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, unsigned short &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_SHORT, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, unsigned int &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONG, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, unsigned long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, unsigned long long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, bool &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_TINY, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, float &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_FLOAT, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, double &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_DOUBLE, value);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, char *value, long size) {
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_VAR_STRING, value, size);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, matador::date &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_DATE, value);
  } else {
    if (info_[result_index_].length > 0) {
      auto *mtt = (MYSQL_TIME*)info_[result_index_].buffer;
      value.set(mtt->day, mtt->month, mtt->year);
    }
    bind_[result_index_].length = nullptr;
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, matador::time &value)
{
  if (mysql::version < 50604) {
    if (prepare_binding_) {
      prepare_bind_column(column_index_++, MYSQL_TYPE_VAR_STRING, value);
    } else {
      if (info_[result_index_].length > 0) {
        // before mysql version 5.6.4 datetime
        // doesn't support fractional seconds
        // so we use a datetime string here
        std::string val;
        read_value(id, index, row, val);
        value = matador::time::parse(val, "%FT%T");
        ++result_index_;
      }
    }
  } else {
    if (prepare_binding_) {
      prepare_bind_column(column_index_++, MYSQL_TYPE_TIMESTAMP, value);
    } else {
      if (info_[result_index_].length > 0) {
        auto *mtt = (MYSQL_TIME*)info_[result_index_].buffer;
        value.set(mtt->year, mtt->month, mtt->day, mtt->hour, mtt->minute, mtt->second, mtt->second_part / 1000);
      }
      bind_[result_index_].length = nullptr;
      ++result_index_;
    }
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, std::string &value)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_STRING, value);
  } else {
    on_truncated_data(result_index_, value);
    ++result_index_;
  }
}

void mysql_prepared_result::read_value(const char *id, int index, int row, std::string &value, long size)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_VAR_STRING, value, size);
  } else {
    if (*bind_[result_index_].error) {
      // assume truncated data
      on_truncated_data(result_index_, value);
    } else {
      auto *data = (char *) bind_[result_index_].buffer;
      unsigned long len = info_[result_index_].length;
      value.assign(data, len);
    }
    ++result_index_;
  }
}

void mysql_prepared_result::on_truncated_data(int index, std::string &x) {
  if (info_[index].length == 0) {
    return;
  }
  bind_[index].buffer = new char[info_[index].length];
  bind_[index].buffer_length = info_[index].length;
  if (mysql_stmt_fetch_column(stmt, &bind_[index], index, 0) != 0) {
    // an error occured
  } else {
    auto *data = (char*)bind_[index].buffer;
    unsigned long len = bind_[index].buffer_length;
    x.assign(data, len);
  }
  delete [] (char*)bind_[index].buffer;
  bind_[index].buffer = nullptr;
  bind_[index].length = nullptr;
}

void mysql_prepared_result::on_belongs_to(const char *id, identifiable_holder &x, cascade_type)
{
  if (prepare_binding_) {
    std::unique_ptr<basic_identifier> pk(x.create_identifier());
    pk->serialize(id, *this);
    foreign_keys_.insert(std::make_pair(id, std::move(pk)));
  } else {
    auto i = foreign_keys_.find(id);
    if (i != foreign_keys_.end()) {
      if (i->second->is_valid()) {
        x.reset(i->second.release());
      }
      foreign_keys_.erase(i);
    }
    ++result_index_;
  }
}

void mysql_prepared_result::on_has_one(const char *id, identifiable_holder &x, cascade_type)
{
  if (prepare_binding_) {
    std::unique_ptr<basic_identifier> pk(x.create_identifier());
    pk->serialize(id, *this);
    foreign_keys_.insert(std::make_pair(id, std::move(pk)));
  } else {
    auto i = foreign_keys_.find(id);
    if (i != foreign_keys_.end()) {
      if (i->second->is_valid()) {
        x.reset(i->second.release());
      }
      foreign_keys_.erase(i);
    }
    ++result_index_;
  }
}

bool mysql_prepared_result::needs_bind()
{
  prepare_binding_ = true;
  column_index_ = 0;
  return prepare_binding_;
}

bool mysql_prepared_result::finalize_bind()
{
  // Todo: validate result
  return mysql_stmt_bind_result(stmt, &bind_.front()) > 0;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, matador::date &)
{
  if (info_[index].buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = (unsigned long)s;
    info_[index].is_allocated = true;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, matador::time &)
{
  if (info_[index].buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = (unsigned long)s;
    info_[index].is_allocated = true;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, std::string & /*value*/)
{
  bind_[index].buffer_type = type;
  bind_[index].buffer = nullptr;
  bind_[index].buffer_length = 0;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, char *x, size_t s)
{
  bind_[index].buffer_type = type;
  bind_[index].buffer= x;
  bind_[index].buffer_length = (unsigned long) s;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, std::string & /*value*/, size_t s)
{
  if (info_[index].buffer == nullptr) {
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = (unsigned long)s;
    info_[index].is_allocated = true;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].buffer = nullptr;
  bind_[index].buffer_length = 0;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

}
}
