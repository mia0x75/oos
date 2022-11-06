#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

#ifdef _MSC_VER
  #ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define MATADOR_OBJECT_API
#endif

#include "matador/utils/identifier_resolver.hpp"

#include "matador/object/object_holder_type.hpp"

#include <ostream>
#include <set>
#include <list>

#include <map>
#include <memory>

namespace matador {

class object_store;
class object_holder;
class prototype_node;
class basic_identifier;
class transaction;

namespace detail {
class basic_relation_data;
}

/**
 * @cond MATADOR_DEV
 * @class object_proxy
 * @brief A proxy between the object and the object_store
 *
 * Is a proxy between the object and the object store. It stores
 * the pointer to the object and the id. Once a new object
 * is inserted into the 
 */
class MATADOR_OBJECT_API object_proxy
{
public:

  /**
   * @brief Create an empty object_proxy
   *
   * Create an empty object_proxy
   */
  object_proxy() = default;

  /**
   * Create a new object proxy with primary key
   *
   * @param pk primary key of object
   */
  explicit object_proxy(basic_identifier *pk);

  template < class T >
  explicit object_proxy(basic_identifier *pk, T *obj, object_store *store, prototype_node *node)
    : object_proxy(pk, obj, 0, store, node)
  {}

  /**
   * @brief Create an object_proxy for a given object.
   *
   * Create an object_proxy for unknown object
   * with given id.
   *
   * @param o The valid object.
   */
  template < typename T >
  explicit object_proxy(T *o)
    : object_proxy(identifier_resolver<T>::resolve(o), o, 0, nullptr, nullptr)
  {}

  /**
   * @brief Create an object_proxy for a given object.
   *
   * Create an object_proxy for unknown object
   * with given id.
   *
   * @param o The valid object.
   * @param id The object store id for the given object
   * @param os The object_store.
   */
  template < typename T >
  object_proxy(T *o, unsigned long id, object_store *store)
    : object_proxy(identifier_resolver<T>::resolve(o), o, id, store, nullptr)
  {}

  template < class T >
  explicit object_proxy(basic_identifier *pk, T *obj, unsigned long id, object_store *store, prototype_node *node)
  : obj_(obj)
  , deleter_(&destroy<T>)
  , namer_(&type_id<T>)
  , oid(id)
  , ostore_(store)
  , node_(node)
  , primary_key_(pk)
  {}

  ~object_proxy();

  /**
   * Return the classname/typeid of the object
   *
   * @return The classname of the object
   */
  const char* classname() const;

  /**
   * Return the underlaying object
   *
   * @tparam The type of the object
   * @return The underlaying object
   */
  template < typename T = void >
  T* obj()
  {
    return static_cast<T*>(obj_);
  }

  /**
   * Return the underlaying object
   *
   * @tparam The type of the object
   * @return The underlaying object
   */
  template < typename T = void >
  const T* obj() const
  {
    return static_cast<const T*>(obj_);
  }

  /**
   * Return the underlaying object store
   *
   * @return The object store
   */
  object_store* ostore() const;

  /**
   * Return the corresponding prototype
   * node.
   *
   * @return The corresponding prototype node
   */
  prototype_node* node() const;

  /**
   * Release the object from proxies
   * responsibility. After release the user
   * is responsible for object.
   *
   * @tparam The type of the object
   * @return The released object
   */
  template < class T >
  T* release()
  {
    T* tmp = obj<T>();
    obj_ = nullptr;
    return tmp;
  }

  /**
   * Print the object_proxy to a stream
   *
   * @param os The stream where the object_proxy is printed to
   * @param op The object_proxy to print
   * @return The modified stream.
   */
  friend MATADOR_OBJECT_API std::ostream& operator <<(std::ostream &os, const object_proxy &op);

  /**
   * Link this object proxy before given
   * object proxy next.
   * 
   * @param successor New next object proxy of this
   */
  void link(object_proxy *successor);

  /**
   * @brief Unlink object proxy from list.
   *
   * When an object_proxy is unlinked it
   * is removed from the object_proxy list
   * contained by the object_store.
   *
   * Maybe the markers are adjusted.
   */
  void unlink();

  /**
   * Increment reference counter
   */
  unsigned long operator++();
  unsigned long operator++(int);
  /**
   * Decrement reference counter
   */
  unsigned long operator--();
  unsigned long operator--(int);
  /**
   * Return true if the object_proxy is linked.
   *
   * @return True if the object_proxy is linked.
   */
  bool linked() const;

  /**
   * Return the next object proxy
   *
   * @return The next object proxy
   */
  object_proxy* next() const;

  /**
   * Return the previous object proxy
   *
   * @return The previous object proxy
   */
  object_proxy* prev() const;


  unsigned long reference_count() const;

  /**
   * Resets the object of the object_proxy
   * with the given object.
   *
   * @param o The new object for the object_proxy
   */
  template < typename T >
  void reset(T *o, bool resolve_identifier = true, bool keep_ref_count = false)
  {
    if (!keep_ref_count) {
      reference_counter_ = 0;
    }
    deleter_ = &destroy<T>;
    namer_ = &type_id<T>;
    obj_ = o;
    oid = 0;
    node_ = nullptr;
    if (obj_ != nullptr && resolve_identifier) {
      primary_key_ = identifier_resolver<T>::resolve(o);
//      primary_key_.reset(identifier_resolver<T>::create());
    }
  }


  /**
   * @brief Add an object_holder to the linked list.
   *
   * Each object_holder containing this object_proxy
   * calls this method. So object_proxy knows how many
   * object_holder are dealing with this object.
   *
   * @param ptr The object_holder containing this object_proxy.
   */
  void add(object_holder *ptr);

  /**
   * @brief Remove an object_holder from the linked list.
   *
   * Each destroying ore reseting object_holder
   * containg this object_proxy calls this method.
   * So object_proxy knows how many object_holder
   * are dealing with this object.
   *
   * @param ptr The object_holder containing this object_proxy.
   */
  bool remove(object_holder *ptr);

  /**
   * @brief True if proxy is valid
   * 
   * Returns true if the proxy is part of
   * an object_store. Therefor ostore, node,
   * prev and next must be set.
   * 
   * @return True if part of an object_store.
   */
  bool valid() const;

  /**
   * Return the id of the object. If no object is
   * set 0 (null) is returned
   *
   * @return 0 (null) or the id of the object.
   */
  unsigned long id() const;

  /**
   * Sets the id of the object_proxy.
   *
   * @param i New id of the proxy.
   */
  void id(unsigned long i);

  /**
   * Returns true if the underlying
   * object has a primary key
   *
   * @return true If the object has a primary key
   */
  bool has_identifier() const;

  /**
   * Return the primary key. If underlaying object
   * doesn't have a primary key, an uninitialized
   * primary key is returned
   *
   * @return The primary key of the underlaying object
   */
  basic_identifier* pk() const;

  void pk(basic_identifier *id);

private:
  transaction current_transaction();
  bool has_transaction() const;

private:
  friend class object_store;
  friend class prototype_node;
  friend class prototype_tree;
  template < class T > friend class result;
  friend class table_reader;
  friend class restore_visitor;
  friend class object_holder;
  template < class T, object_holder_type OHT > friend class object_pointer;
  friend class detail::basic_relation_data;
  typedef void (*deleter)(void*);
  typedef const char* (*namer)();

  template <typename T>
  static void destroy(void* p)
  {
    delete (T*)p;
  }

  template < class T >
  static const char* type_id()
  {
    return typeid(T).name();
  }

  object_proxy *prev_ = nullptr;      /**< The previous object_proxy in the list. */
  object_proxy *next_ = nullptr;      /**< The next object_proxy in the list. */

  void *obj_ = nullptr;         /**< The concrete object. */
  deleter deleter_ = nullptr;             /**< The object deleter function */
  namer namer_ = nullptr;       /**< The object classname function */
  unsigned long oid = 0;        /**< The id of the concrete or expected object. */

  unsigned long reference_counter_ = 0;

  object_store *ostore_ = nullptr;    /**< The object_store to which the object_proxy belongs. */
  prototype_node *node_ = nullptr;    /**< The prototype_node containing the type of the object. */

  typedef std::set<object_holder *> ptr_set_t; /**< Shortcut to the object_holder set. */
  ptr_set_t ptr_set_;      /**< This set contains every object_holder pointing to this object_proxy. */
  
  basic_identifier *primary_key_ = nullptr;
};
/// @endcond
}

#endif /* OBJECT_PROXY_HPP */
