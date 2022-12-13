#ifndef MATADOR_RELATION_FIELD_ENDPOINT_HPP
#define MATADOR_RELATION_FIELD_ENDPOINT_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
#define MATADOR_OBJECT_API __declspec(dllexport)
#define EXPIMP_OBJECT_TEMPLATE
#else
#define MATADOR_OBJECT_API __declspec(dllimport)
#define EXPIMP_OBJECT_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#else
#define MATADOR_OBJECT_API
#endif

#include "matador/utils/is_builtin.hpp"

#include "matador/object/relation_endpoint_value_inserter.hpp"
#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/basic_has_many_to_many_item.hpp"
#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include <string>
#include <functional>
#include <memory>
#include <iosfwd>

namespace matador {

class object_proxy;
class prototype_node;
class object_store;

namespace detail {

/// @cond MATADOR_DEV

/**
 * Basic relation endpoint providing the interface
 */
struct MATADOR_OBJECT_API basic_relation_endpoint : public object_proxy_accessor
{
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  basic_relation_endpoint(std::string fld, prototype_node *n, relation_type t)
    : field(std::move(fld)), node(n), type(t)
  {
    switch (type) {
      case BELONGS_TO: type_name = "belongs_to"; break;
      case HAS_ONE: type_name = "has_one"; break;
      case HAS_MANY: type_name = "has_many"; break;
      default: break;
    }
  }
  virtual ~basic_relation_endpoint() = default;

  virtual void insert_value(object_proxy *value, object_proxy *owner) = 0;
  virtual void remove_value(object_proxy *value, object_proxy *owner) = 0;
  virtual void insert_value(const basic_has_many_item_holder &value, object_proxy *owner) = 0;
  virtual void remove_value(const basic_has_many_item_holder &value, object_proxy *owner) = 0;

  void insert_value_into_foreign(object_proxy *value, object_proxy *owner);
  template < class Value >
  void insert_value_into_foreign(const has_many_item_holder<Value> &holder, object_proxy *owner);
  void remove_value_from_foreign(object_proxy *value, object_proxy *owner);
  template < class Value >
  void remove_value_from_foreign(const has_many_item_holder<Value> &value, object_proxy *owner);

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, const object_holder &obj);
  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, object_proxy *proxy);


  void increment_reference_count(const object_holder &holder);
  void decrement_reference_count(const object_holder &holder);

  void mark_holder_as_inserted(basic_has_many_item_holder &holder) const;
  void mark_holder_as_removed(basic_has_many_item_holder &holder) const;

  virtual void print(std::ostream &out) const;

  std::string field;
  std::string type_name;
  prototype_node *node = nullptr;
  relation_type type;
  std::weak_ptr<basic_relation_endpoint> foreign_endpoint;
};

std::ostream& operator<<(std::ostream &stream, const basic_relation_endpoint &endpoint);

/**
 * relation endpoint interface with value type
 * @tparam Value
 */
template < class Value >
struct relation_endpoint : public basic_relation_endpoint
{
  relation_endpoint(const std::string &field, prototype_node *node, relation_type type)
    : basic_relation_endpoint(field, node, type)
  {}

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) = 0;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) = 0;

  virtual object_proxy* acquire_proxy(unsigned long oid, object_store &store) = 0;
};

/**
 * Base class for to_many relation endpoints
 * providing value and owner type
 *
 * @tparam Value
 * @tparam Owner
 */
template < class Value, class Owner >
struct from_many_endpoint : public relation_endpoint<Value>
{
  from_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  std::string owner_column;
  std::string item_column;
};

template < class Value, class Owner, basic_relation_endpoint::relation_type Type, class Enabled = void >
struct from_one_endpoint;

/**
 * Base class for from one relation endpoints
 * like 'has_one' and 'belongs_to_one' endpoints
 *
 * @tparam Value
 * @tparam Owner
 * @tparam Type
 */
template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
struct from_one_endpoint<Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  from_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, Type)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long , object_store &) override { return nullptr; }
};

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
struct from_one_endpoint<Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  from_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, Type)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long , object_store &) override { return nullptr; }
};

template < class Value, class Owner >
using has_one_endpoint = from_one_endpoint<Value, Owner, basic_relation_endpoint::HAS_ONE >;

template < class Value, class Owner >
using belongs_to_one_endpoint = from_one_endpoint<Value, Owner, basic_relation_endpoint::BELONGS_TO >;

template < class Value, class Owner, class Enabled = void >
struct many_to_one_endpoint;

template < class Value, class Owner >
struct many_to_one_endpoint<Value, Owner, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type>
  : public relation_endpoint<Value>
{
  typedef has_many_item_holder<Value> value_type;

  many_to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long , object_store &) override { return nullptr; }

};

template < class Value, class Owner >
struct many_to_one_endpoint<Value, Owner, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, Value>::value>::type>
  : public relation_endpoint<Value>
{
  many_to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  { }

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &, has_many_item_holder<Value> &, object_proxy *) override {} // owner
  void remove_holder(object_store &, has_many_item_holder<Value> &, object_proxy *) override {} // owner

  void insert_value(object_proxy *, object_proxy *) override {} // value, owner
  void remove_value(object_proxy *, object_proxy *) override {} // value, owner

  void insert_value(const basic_has_many_item_holder &, object_proxy *) override {} // owner
  void remove_value(const basic_has_many_item_holder &, object_proxy *) override {} // owner

  object_proxy* acquire_proxy(unsigned long , object_store &) override { return nullptr; }
};

template < class Value, class Owner, class Enabled = void >
struct belongs_to_many_endpoint;

template < class Value, class Owner >
struct belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  belongs_to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::BELONGS_TO)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long , object_store &) override { return nullptr; }

  void print(std::ostream &out) const override;
};

template < class Value, class Owner >
struct belongs_to_many_endpoint<Value, Owner, typename std::enable_if<matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  belongs_to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::BELONGS_TO)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &, has_many_item_holder<Value> &, object_proxy *) override {} // owner
  void remove_holder(object_store &, has_many_item_holder<Value> &, object_proxy *) override {} // owner

  void insert_value(object_proxy *, object_proxy *) override {} // value, owner
  void remove_value(object_proxy *, object_proxy *) override {} // value, owner

  void insert_value(const basic_has_many_item_holder &, object_proxy *) override {} // owner
  void remove_value(const basic_has_many_item_holder &, object_proxy *) override {} // owner

  object_proxy* acquire_proxy(unsigned long , object_store &) override { return nullptr; }
};
/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
