#ifndef OBJECT_HOLDER_HPP
#define OBJECT_HOLDER_HPP

#include "matador/object/export.hpp"

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include <memory>

namespace matador {

/// @cond MATADOR_DEV

namespace detail {
class object_inserter;
class object_deleter;
class object_proxy_accessor;
struct basic_relation_endpoint;
template < class Value >
class relation_endpoint_value_inserter;
template < class Value >
class relation_endpoint_value_remover;
}

/// @endcond

class object_proxy;
class object_store;

/**
 * @class object_holder
 * @brief Base class for the object pointer and reference class
 *
 * This is the base class for the object pointer
 * and reference class. The class holds the proxy
 * of the object and the id of the object.
 */
class MATADOR_OBJECT_API object_holder : public identifiable_holder
{
protected:
  /**
   * @brief Creates and empty base pointer.
   * 
   * Creates and empty base pointer. The boolean
   * tells the class if the object is handled
   * as a reference or an pointer. The difference
   * is that the reference couldn't be deleted
   * from the object_store and the pointer can.
   */
  object_holder() = default;

  /**
   * Copies from another object_holder
   * 
   * @param x the object_holder to copy from.
   */
  object_holder(const object_holder &x);

  /**
   * Copy move constructor
   *
   * @param x object holder to copy move
   */
  object_holder(object_holder &&x) noexcept;

  /**
   * Assign operator.
   * 
   * @param x The object_holder to assign from.
   */
  object_holder& operator=(const object_holder &x);

  /**
   * Assign move constructor
   *
   * @param x object holder to assign move
   * @return The moved object holder
   */
  object_holder& operator=(object_holder &&x) noexcept;

  /**
   * @brief Creates an object_holder with a given object_proxy
   * 
   * Creates an object_holder with a given object_proxy. The
   * boolean tells the object_holder if it should be
   * handled as an internal.
   * 
   * @param proxy The object_proxy of the object_holder
   */
  explicit object_holder(object_proxy *proxy);

  /**
   * Destroys the object_holder
   * and decides whether the underlying
   * object_proxy is destroyed as well.
   *
   * It is destroyed if it is not inserted
   * into any object_store.
   */
  ~object_holder() override;

public:

  /**
   * Equal to operator for the object_holder
   * 
   * @param x The object_holder to check equality with.
   * @return True if the holder objects are the same
   */
  bool operator==(const object_holder &x) const;

  /**
   * Checks equality with a nullptr
   *
   * @return True if the holder objects is nullptr
   */
  bool operator==(std::nullptr_t) const;

  /**
   * Not equal to operator for the object_holder
   * 
   * @param x The object_holder to check unequality with.
   * @return True if the holder objects are not the same
   */
  bool operator!=(const object_holder &x) const;

  /**
   * Checks equality with a nullptr
   *
   * @return True if the holder objects isn't nullptr
   */
  bool operator!=(std::nullptr_t) const;

  /**
   * Returns true if holder holds a valid
   * proxy/object.
   *
   * @return True if proxy/object is not nullptr
   */
  explicit operator bool() const noexcept;

  /**
   * Resets the object_holder with the given object_proxy.
   * 
   * @param proxy The new object_proxy for the object_holder.
   * @param cascade Sets the cascade actions for the proxy.
   */
  void reset(object_proxy *proxy, cascade_type cascade);

  /**
   * Resets the object_holder with the given object_proxy.
   *
   * @param proxy The new object_proxy for the object_holder.
   * @param cascade Sets the cascade actions for the proxy.
   * @param notify_foreign_relation True if foreign relation endpoint should be modified
   */
  void reset(object_proxy *proxy, cascade_type cascade, bool notify_foreign_relation);

  /**
   * Resets the object holder with the given
   * object holder.
   *
   * @param holder The object holder to be used to reset
   */
  void reset(object_holder &holder);

  /**
   * Resets the object_holder with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  void reset(const identifier &id) override;

  /**
   * Clears the currently set object
   */
  void clear();

  /**
   * Returns true if object_holder doesn't
   * holds an object
   *
   * @return True if object_holder doesn't holds an object
   */
  bool empty() const noexcept;

  /**
   * Returns true if object_holder
   * holds a valid object
   *
   * @return True if object_holder holds an object
   */
  bool valid() const noexcept;
  /**
   * Returns if the object is loaded.
   * 
   * @return True if the object is loaded.
   */
  bool is_loaded() const;

  /**
   * Returns the object id.
   * 
   * @return The id of the object.
   */
  unsigned long long id() const;

  /**
   * Returns the corresponding
   * object_store or nullptr
   */
  object_store* store() const;

  /**
   * Returns the raw object pointer
   * 
   * @return The raw object pointer.
   */
  void* ptr();

  /**
   * Returns the raw object pointer
   *
   * @return The raw object pointer.
   */
  const void* ptr() const;

  /**
   * Returns the object pointer
   * 
   * @return The object pointer.
   */
  void* lookup_object();

  /**
   * Returns the object pointer
   *
   * @return The object pointer.
   */
  void* lookup_object() const;

  /**
   * Returns if this object_holder is inside
   * of the object_store. This is important
   * to calculate the reference and pointer
   * counter.
   * 
   * @return True if the object_holder internal
   */
  bool is_internal() const;

  /**
   * Returns true if the underlying object
   * is inserted in an object_store
   *
   * @return True if object is inserted.
   */
  bool is_inserted() const;

  /**
   * Returns true if object has a primary key
   *
   * @return true if object has a primary key
   */
  bool has_primary_key() const override ;

  /**
   * Gets the primary key of the foreign object
   *
   * @return The primary key of the foreign object
   */
  const identifier& primary_key() const override;
  identifier& primary_key() override;

  /**
   * Returns the current reference count
   *
   * @return The current reference count
   */
  unsigned long reference_count() const;

  /**
   * Return the type string of the object
   *
   * @return The type string of the object.
   */
  virtual const char* type() const = 0;

  /**
   * Returns the cascade type of the holder
   *
   * @return The cascade type
   */
  cascade_type cascade() const;

  /**
   * Prints the underlying object
   *
   * @param out The output stream to write on.
   * @param x The object pointer to print.
   * @return The output stream.
   */
  friend MATADOR_OBJECT_API std::ostream& operator<<(std::ostream &out, const object_holder &x);

private:
  friend class object_serializer;
  friend class object_proxy;
  friend class detail::object_deleter;
  friend class detail::object_inserter;
  friend class object_store;
  friend class abstract_has_many;
  friend class detail::object_proxy_accessor;
  template < class T, class Enable >
  friend class has_many_item_holder;
  template < class L, class R >
  friend class has_many_to_many_item;

  // Todo: change interface to remove friend
  friend class session;

  template < class T >
  friend class object_ptr;
  friend struct detail::basic_relation_endpoint;

  object_proxy *proxy_ = nullptr;
  object_proxy *owner_ = nullptr; // only set if holder type is BELONGS_TO or HAS_MANY
  cascade_type cascade_ = cascade_type::NONE;
  bool is_inserted_ = false;

  std::shared_ptr<detail::basic_relation_endpoint> relation_info_;
};

}

#endif //OBJECT_HOLDER_HPP
