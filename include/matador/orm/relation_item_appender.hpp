#ifndef OOS_RELATION_TABLE_LOADER_HPP
#define OOS_RELATION_TABLE_LOADER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"

#include <cstddef>
#include <iostream>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

template < class OWNER >
class relation_item_appender
{
public:

  void append(const std::string &id, detail::t_identifier_map &identifier_proxy_map, basic_table::t_relation_item_map *has_many_relations)
  {
    has_many_relations_ = has_many_relations;
    owner_id_ = id;
    for (auto value : identifier_proxy_map) {
      id_ptr_ = value.first;
      matador::access::serialize(*this, *value.second->obj<OWNER>());
    }
    owner_id_.clear();
    identifier_proxy_map.clear();
    has_many_relations_ = nullptr;
  }

  template<class T>
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template<class T>
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) { }
  void serialize(const char *, std::string &, size_t) { }

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) { }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &x, const char *, const char *, cascade_type cascade)
  {
    serialize(id, x, cascade);
  }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &, cascade_type)
  {
    if (owner_id_ != id) {
      return;
    }
    auto rel = has_many_relations_->find(id);
    if (rel == has_many_relations_->end()) {
      return;
    }
    auto items = rel->second.equal_range(id_ptr_);
    for (auto item = items.first; item != items.second; ++item) {
//      typename basic_has_many<V, C>::internal_type val(item->second);
//      x.append(val);
    }
    // clear all elements
    rel->second.erase(id_ptr_);
  }

private:
  std::string owner_id_;
  basic_identifier* id_ptr_;
  basic_table::t_relation_item_map *has_many_relations_ = nullptr;
};

/// @endcond

}
}
#endif //OOS_RELATION_TABLE_LOADER_HPP
