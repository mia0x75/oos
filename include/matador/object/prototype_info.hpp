#ifndef MATADOR_PROTOTYPE_INFO_HPP
#define MATADOR_PROTOTYPE_INFO_HPP

#include "matador/object/typed_object_store_observer.hpp"
#include "matador/object/relation_field_endpoint.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

namespace matador {

/// @cond MATADOR_DEV

class prototype_node;

namespace detail {

enum class notification_type {
  ATTACH,
  DETACH,
  INSERT,
  UPDATE,
  REMOVE
};

class abstract_prototype_info
{
public:
  virtual ~abstract_prototype_info() = default;

protected:
  abstract_prototype_info(prototype_node &n, std::type_index tindex)
    : node(n), type_index_(tindex)
  {}

public:
  typedef std::unordered_map<std::type_index, std::shared_ptr<detail::basic_relation_endpoint>> t_endpoint_map;
  typedef t_endpoint_map::iterator endpoint_iterator;
  typedef t_endpoint_map::const_iterator const_endpoint_iterator;

public:
  std::type_index type_index() const { return type_index_; }

  void register_relation_endpoint(const std::type_index &tindex, const std::shared_ptr<basic_relation_endpoint> &endpoint);
  void unregister_relation_endpoint(const std::type_index &tindex);

  const_endpoint_iterator find_relation_endpoint(const std::type_index &tindex) const;
  endpoint_iterator find_relation_endpoint(const std::type_index &tindex);

  const_endpoint_iterator find_relation_endpoint(const std::string &field) const;
  endpoint_iterator find_relation_endpoint(const std::string &field);

  endpoint_iterator endpoint_begin();
  const_endpoint_iterator endpoint_begin() const;

  endpoint_iterator endpoint_end();
  const_endpoint_iterator endpoint_end() const;

  std::size_t endpoints_size() const;
  bool endpoints_empty() const;

  const t_endpoint_map& endpoints() const;

  virtual void* prototype() const = 0;
  virtual void* create() const = 0;
  virtual void register_observer(basic_object_store_observer *obs) = 0;
  virtual void notify(notification_type type, object_proxy *proxy = nullptr) const = 0;

protected:
  prototype_node &node;        /**< prototype node of the represented object type */
  std::type_index type_index_; /**< type index of the represented object type */

  t_endpoint_map endpoints_;
};

template < class T >
class basic_prototype_info : public abstract_prototype_info
{
protected:
  basic_prototype_info(prototype_node &node, T *proto)
    : abstract_prototype_info(node, std::type_index(typeid(T)))
    , prototype_(proto)
  {}

  void* prototype() const override;
  void register_observer(basic_object_store_observer *obs) override;
  void notify(notification_type type, object_proxy *proxy) const override;

  T* get() const
  {
    return static_cast<T*>(prototype());
  }

protected:
  std::unique_ptr<T> prototype_;
  typedef std::vector<std::unique_ptr<matador::typed_object_store_observer<T>>> t_observer_vector;
  t_observer_vector observers;
};

template < class T >
void* basic_prototype_info<T>::prototype() const
{
  return prototype_.get();
}

template < class T >
void basic_prototype_info<T>::register_observer(basic_object_store_observer *obs)
{
  observers.emplace_back(std::unique_ptr<typed_object_store_observer<T>>(static_cast<typed_object_store_observer<T>*>(obs)));
}

template < class T >
void basic_prototype_info<T>::notify(notification_type type, object_proxy *proxy) const
{
  for(auto &observer : observers) {
    switch (type) {
      case notification_type::ATTACH:
        observer->on_attach(node, *prototype_);
        break;
      case notification_type::DETACH:
        observer->on_detach(node, *prototype_);
        break;
      case notification_type::INSERT:
        observer->on_insert(*proxy);
        break;
      case notification_type::UPDATE:
        observer->on_update(*proxy);
        break;
      case notification_type::REMOVE:
        observer->on_delete(*proxy);
        break;
      default:
        break;
    }
  }
}

template < class T, class Enabled = void >
class prototype_info;

template < class T >
class prototype_info<T, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, T>::value>::type> : public basic_prototype_info<T>
{
public:
  prototype_info(prototype_node &node, T *proto)
    : basic_prototype_info<T>(node, proto)
  {}

  void* create() const override
  {
    return new T;
  }

};

template < class T >
class prototype_info<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value>::type> : public basic_prototype_info<T>
{
public:
  prototype_info(prototype_node &node, T *proto)
    : basic_prototype_info<T>(node, proto)
  {}

  void* create() const override
  {
    return new T(this->get()->left_column(), this->get()->right_column());
  }
};

/// @endcond

}
}
#endif //MATADOR_PROTOTYPE_INFO_HPP
