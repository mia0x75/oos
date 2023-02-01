//
// Created by sascha on 3/24/16.
//

#include "matador/orm/persistence.hpp"

namespace matador {

bool is_loaded(const object_holder &holder)
{
  return holder.is_loaded();
}

basic_table::basic_table(prototype_node &node, persistence &p)
  : persistence_(p)
  , node_(node)
{ }

std::string basic_table::name() const
{
  return node_.type();
}

bool basic_table::is_loaded() const
{
  return is_loaded_;
}

prototype_node &basic_table::node()
{
  return node_;
}

const prototype_node &basic_table::node() const
{
  return node_;
}

void basic_table::reset()
{
  is_loaded_ = false;
}

persistence& basic_table::persistence_unit()
{
  return persistence_;
}

const matador::persistence &basic_table::persistence_unit() const
{
  return persistence_;
}

basic_table::t_table_map::iterator basic_table::find_table(const std::string &type)
{
  return persistence_.find_table(type);
}

basic_table::t_table_map::iterator basic_table::begin_table()
{
  return persistence_.begin();
}

basic_table::t_table_map::iterator basic_table::end_table()
{
  return persistence_.end();
}

detail::t_identifier_proxy_infos::iterator
basic_table::insert_proxy(const identifier &pk, object_proxy *proxy)
{
  return identifier_proxy_map_.insert(std::make_pair(pk, detail::object_proxy_info(proxy))).first;
}

detail::t_identifier_proxy_infos::iterator basic_table::find_proxy(const identifier &pk)
{
  return identifier_proxy_map_.find(pk);
}

detail::t_identifier_proxy_infos::iterator basic_table::begin_proxy()
{
  return identifier_proxy_map_.begin();
}

detail::t_identifier_proxy_infos::iterator basic_table::end_proxy()
{
  return identifier_proxy_map_.end();
}

basic_table::t_relation_data_map::iterator
basic_table::find_relation_data(const char *id)
{
  return relation_data_map_.find(id);
}

basic_table::t_relation_data_map::iterator
basic_table::begin_relation_data()
{
  return relation_data_map_.begin();
}

basic_table::t_relation_data_map::iterator
basic_table::end_relation_data()
{
  return relation_data_map_.end();
}
}