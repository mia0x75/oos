#include "matador/object/prototype_info.hpp"
#include "matador/object/prototype_node.hpp"

#include <algorithm>

namespace matador {
namespace detail {

void abstract_prototype_info::register_relation_endpoint(const std::type_index &tindex,
                                                      const std::shared_ptr<basic_relation_endpoint> &endpoint)
{
  endpoints_.insert(std::make_pair(tindex, endpoint));
}

void abstract_prototype_info::unregister_relation_endpoint(const std::type_index &tindex)
{
  endpoints_.erase(tindex);
}

abstract_prototype_info::const_endpoint_iterator abstract_prototype_info::find_relation_endpoint(const std::type_index &tindex) const
{
  return endpoints_.find(tindex);
}

abstract_prototype_info::endpoint_iterator abstract_prototype_info::find_relation_endpoint(const std::type_index &tindex)
{
  return endpoints_.find(tindex);
}

abstract_prototype_info::const_endpoint_iterator abstract_prototype_info::find_relation_endpoint(const std::string &field) const
{
  return std::find_if(endpoints_.begin(), endpoints_.end(), [&field](const t_endpoint_map::value_type &value) {
    return value.second->field == field;
  });
}

abstract_prototype_info::endpoint_iterator abstract_prototype_info::find_relation_endpoint(const std::string &field)
{
  return std::find_if(endpoints_.begin(), endpoints_.end(), [&field](const t_endpoint_map::value_type &value) {
    return value.second->field == field;
  });
}

abstract_prototype_info::endpoint_iterator abstract_prototype_info::endpoint_begin()
{
  return endpoints_.begin();
}

abstract_prototype_info::const_endpoint_iterator abstract_prototype_info::endpoint_begin() const
{
  return endpoints_.begin();
}

abstract_prototype_info::endpoint_iterator abstract_prototype_info::endpoint_end()
{
  return endpoints_.end();
}

abstract_prototype_info::const_endpoint_iterator abstract_prototype_info::endpoint_end() const
{
  return endpoints_.end();
}

const abstract_prototype_info::t_endpoint_map &abstract_prototype_info::endpoints() const
{
  return endpoints_;
}

std::size_t abstract_prototype_info::endpoints_size() const
{
  return endpoints_.size();
}

bool abstract_prototype_info::endpoints_empty() const
{
  return endpoints_.empty();
}
}
}