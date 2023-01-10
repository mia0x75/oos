#ifndef OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
#define OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP

#include "matador/sql/export.hpp"

#include "matador/utils/time.hpp"
#include "matador/utils/any_visitor.hpp"

#include "matador/sql/column.hpp"

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API query_value_column_processor
{
public:
  query_value_column_processor(std::shared_ptr<columns> update_columns, std::vector<matador::any> rowvalues);

  void execute(std::pair<std::string, matador::any> &a);

private:
  template < class T >
  void process(T &val)
  {
    std::shared_ptr<detail::value_column<T>> ival(new detail::value_column<T>(current_id_, val));
    update_columns_->push_back(ival);
  }
  void process(char *val);
  void process(const char *val);

private:
  any_visitor visitor_;
  std::shared_ptr<columns> update_columns_;
  std::vector<matador::any> rowvalues_;
  std::string current_id_;
};

/// @endcond

}
}
#endif //OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
