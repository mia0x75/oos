#include "matador/object/node_analyzer.hpp"
#include "matador/object/prototype_iterator.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/to_many_endpoints.hpp"

namespace matador {
namespace detail {

/*
 * no observer version
 */
template<class Owner, template < class U = Owner > class Observer >
void node_analyzer<Owner, Observer>::analyze()
{
  Owner obj;
  analyze(obj);
}

template<class Owner, template < class U = Owner > class Observer >
void node_analyzer<Owner, Observer>::analyze(Owner &obj)
{
  matador::access::serialize(*this, obj);
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(Value &x)
{
  matador::access::serialize(*this, x);
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(const char *id, belongs_to <Value> &x, cascade_type)
{
  // find foreign_node of belongs to type
  prototype_iterator foreign_node = store_.find(x.type());
  if (foreign_node != store_.end()) {
    /*
     * foreign_node is already inserted
     * check if foreign_node was created from has_many
     * check if foreign_node has_many relation for id (id == tablename)
     */
    auto i = foreign_node->find_endpoint(node_.type_index());

    auto eps = foreign_node->endpoints();

    if (i != foreign_node->endpoint_end()) {

      if (i->second->type == detail::basic_relation_endpoint::HAS_MANY) {
        // yes, foreign_node was created from has_many!
        // detach current foreign_node (has_many_item == relation table)
        store_.detach(node_.type());

        auto foreign_endpoint = std::make_shared<detail::many_to_one_endpoint<Owner, Value>>(i->second->field, foreign_node.get());
        foreign_node->unregister_relation_endpoint(node_.type_index());
        foreign_node->register_relation_endpoint(node_.type_index(), foreign_endpoint);

        auto endpoint = std::make_shared<detail::belongs_to_many_endpoint<Value, Owner>>(id, &node_);
        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

        endpoint->foreign_endpoint = foreign_endpoint;
        foreign_endpoint->foreign_endpoint = endpoint;
      } else if (i->second->type == detail::basic_relation_endpoint::HAS_ONE) {
        // foreign_node was created from has_one
        // check if foreign_node is set
        auto endpoint = std::make_shared<detail::belongs_to_one_endpoint<Value, Owner>>(id, &node_);

        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
        endpoint->foreign_endpoint = i->second;
        i->second->foreign_endpoint = endpoint;
      } else {
        throw_object_exception("foreign node is already a belongs to endpoint (node: " << node_.type() << ", field: " << id << ")");
      }
    } else {
      auto endpoint = std::make_shared<detail::belongs_to_one_endpoint<Value, Owner>>(id, &node_);

      node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
    }
  } else {
    auto endpoint = std::make_shared<detail::belongs_to_one_endpoint<Value, Owner>>(id, &node_);

    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_one <Value> &x, cascade_type)
{
  auto endpoint = std::make_shared<detail::has_one_endpoint<Value, Owner>>(id, &node_);
  node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

  prototype_iterator foreign_node = store_.find(x.type());

  if (foreign_node != store_.end()) {
    auto i = foreign_node->find_endpoint(node_.type_index());
    if (i != foreign_node->endpoint_end()) {
      if (i->second->type == basic_relation_endpoint::BELONGS_TO) {
        // link both nodes
        endpoint->foreign_endpoint = i->second;
        i->second->foreign_endpoint = endpoint;
      } else {
        throw object_exception("invalid endpoint type");
      }
    }
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_many<Value, Container> &x, cascade_type cascade)
{
  if (is_builtin<Value>::value) {
    this->serialize(id, x, node_.type(), typeid(Value).name(), cascade);
  } else {
    auto value_node = store_.find<Value>();
    if (value_node == store_.end()) {
      this->serialize(id, x, node_.type(), typeid(Value).name(), cascade);
    } else {
      this->serialize(id, x, node_.type(), value_node->type(), cascade);
    }
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_many <Value, Container> &,
                                               const char *left_column, const char *right_column,
                                               cascade_type,
                                               typename std::enable_if<!is_builtin<Value>::value>::type*)
{
  // attach relation table for has many relation
  // check if has-many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    // Value = foreign/owner type => left column
    // Owner = item/value type    => right column

    /*
     * Value = value type
     * Owner = owner type
     * new has_many_to_many<Value, Owner>
     */
    // handle observer
    using has_many_item = has_one_to_many_item<Owner, Value>;
    std::vector<Observer<has_many_item>*> has_many_item_observer;
    for (const auto &o : observer_vector_) {
      has_many_item_observer.push_back(new Observer<has_many_item>(o));
    }

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint <Owner, typename has_many_item::right_value_type>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(typename has_many_item::right_value_type)), endpoint);

    // new has many to many item
    auto proto = new has_many_item(left_column, right_column);
    prototype_node *node = prototype_node::make_relation_node<has_many_item>(&store_, id, proto, false, node_.type(), id);

    pi = store_.attach_internal<has_many_item>(node, nullptr, has_many_item_observer);

    auto sep = pi->find_endpoint(left_column);
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
    }
  } else {
    /*
     * switch left (Owner) and right (Value) template parameter
     * to fit the already inserted has_many_to_many_item
     * template parameter
     */

    /*
     * Value = value type
     * Owner = owner type
     * new has_many_to_many<Value, Owner>
     */
    std::type_index ti(typeid(has_one_to_many_item<Value, Owner>));
    if (pi->type_index() == ti) {

      prototype_iterator foreign_node = detach_one_to_many_node<Value>(pi);

      auto foreign_endpoint = std::make_shared<detail::right_to_many_endpoint <Owner, Value>>(id, foreign_node.get());

      foreign_node->register_relation_endpoint(std::type_index(typeid(Owner)), foreign_endpoint);

      std::vector<Observer<has_many_to_many_item<Owner, Value> >*> has_many_item_observer;
      for (auto o : observer_vector_) {
        has_many_item_observer.push_back(new Observer<has_many_to_many_item<Owner, Value> >(o));
      }

      auto endpoint = std::make_shared<detail::left_to_many_endpoint<Value, Owner>>(id, &node_);

      node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

      // link endpoints
      foreign_endpoint->foreign_endpoint = endpoint;
      endpoint->foreign_endpoint = foreign_endpoint;

      // new has many to many item
      auto proto = new has_many_to_many_item<Owner, Value>(right_column, left_column);
      prototype_node *node = prototype_node::make_relation_node<has_many_to_many_item<Owner, Value>>(&store_, id, proto, false, node_.type(), id);

      pi = store_.attach_internal<has_many_to_many_item<Owner, Value>>(node, nullptr, has_many_item_observer);

      auto sep = pi->find_endpoint(right_column);
      if (sep != pi->endpoint_end()) {
        sep->second->foreign_endpoint = endpoint;
      }
      sep = pi->find_endpoint(left_column);
      if (sep != pi->endpoint_end()) {
        sep->second->foreign_endpoint = foreign_endpoint;
      }
    } else {

      // found corresponding belongs_to
      auto j = pi->find_endpoint(node_.type_index());
      if (j == pi->endpoint_end()) {
        // check for has many item
        throw_object_exception("prototype already inserted: " << pi->type());
      } else if (j->second->type == detail::basic_relation_endpoint::BELONGS_TO) {
        // replace foreign endpoint
        auto foreign_endpoint = std::make_shared<detail::belongs_to_many_endpoint <Owner, Value>>(j->second->field, pi.get());
        pi->unregister_relation_endpoint(node_.type_index());
        pi->register_relation_endpoint(node_.type_index(), foreign_endpoint);

        // create and register endpoint
        // here
        auto endpoint = std::make_shared<detail::many_to_one_endpoint <Value, Owner>>(id, &node_);
        endpoint->foreign_endpoint = foreign_endpoint;
        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

        // link both endpoints
        foreign_endpoint->foreign_endpoint = endpoint;
      }
    }
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_many <Value, Container> &,
                                               const char *left_column, const char *right_column,
                                               cascade_type,
                                               typename std::enable_if<is_builtin<Value>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    using has_many_item = has_one_to_many_item<Owner, Value>;
    std::vector<Observer<has_many_item >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new Observer<has_many_item >(o));
    }

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint <Owner, typename has_many_item::right_value_type >>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(typename has_many_item::right_value_type)), endpoint);

    auto proto = new has_many_item(left_column, right_column);
    prototype_node *node = prototype_node::make_relation_node<has_many_item >(&store_, id, proto, false, node_.type(), id);

    pi = store_.attach_internal<has_many_item >(node, nullptr, has_many_item_observer);

    auto sep = pi->find_endpoint(left_column);
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
      endpoint->foreign_endpoint = sep->second;
    }
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
prototype_iterator node_analyzer<Owner, Observer>::detach_one_to_many_node(prototype_iterator node)
{
  // found has one-to-many
  // must be detached because
  // we have a many-to-many relation here
  store_.detach(node);
  // remove registered endpoint from foreign site
  prototype_iterator foreign_node = store_.find<Value>();
  if (foreign_node != store_.end()) {
    foreign_node->unregister_relation_endpoint(node_.type_index());
  }
  return foreign_node;
}

void analyze_has_one(const char *type);
void analyze_belongs_to(const char *type);

}
}