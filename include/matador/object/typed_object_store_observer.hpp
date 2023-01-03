#ifndef OBJECT_STORE_OBSERVER_HPP
#define OBJECT_STORE_OBSERVER_HPP

#include <typeindex>

namespace matador {

class object_proxy;
class prototype_node;

/**
 * Base class for all object store observer
 */
class basic_object_store_observer
{
protected:
  /**
   * Constructs a basic_object_store_observer for the
   * give std::type_index.
   *
   * @param tindex type index of the observer
   */
  explicit basic_object_store_observer(const std::type_index &tindex)
  : type_index_(tindex)
  {}

public:
  virtual ~basic_object_store_observer() = default;

  /**
   * Return the type index of the observer.
   *
   * @return The type index of the observer.
   */
  const std::type_index& index() const { return type_index_; }

private:
  std::type_index type_index_;
};

/**
 * @class typed_object_store_observer
 * @tparam T Type of the typed object store observer
 * @brief Base class for typed object observer classes
 * 
 * When interested to observe
 * - attach (prototype_node)
 * - detach (prototype_node)
 * - insert (object)
 * - update (object)
 * - delete (object)
 * actions an observer class instance must be
 * registered with object store.
 * Use this class as base class for all observer classes.
 */
template < class T >
class typed_object_store_observer : public basic_object_store_observer
{
public:

  /**
   * Default constructor
   */
  typed_object_store_observer() : basic_object_store_observer(std::type_index(typeid(T))) {}

  /**
   * @brief Called on prototype_node attach
   *
   * When a prototype node is attached to the object store
   * this is called after the attaching succeeded.
   *
   * @param node The attached prototype node
   * @param prototype The prototype object of the attached node
   */
  virtual void on_attach(prototype_node &node, T &prototype) = 0;

  /**
   * @brief Called on prototype_node detach
   *
   * When a prototype node is detached from the object store
   * this is called before the detaching succeeded.
   *
   * @param node The to be detached prototype node
   * @param prototype The prototype object of the detached node
   */
  virtual void on_detach(prototype_node &node, T &prototype) = 0;

  /**
   * @brief Called on object insertion.
   * 
   * Called when an object is inserted
   * into the object_store.
   * 
   * @param proxy The proxy of the inserted object.
   */
  virtual void on_insert(object_proxy &proxy) = 0;

  /**
   * @brief Called on object update.
   * 
   * Called when an object is updated
   * in the object_store.
   * 
   * @param proxy The proxy of the updated object.
   */
  virtual void on_update(object_proxy &proxy) = 0;

  /**
   * @brief Called on object deletion.
   * 
   * Called when an object is deleted
   * from the object_store.
   * 
   * @param proxy The proxy of the deleted object.
   */
  virtual void on_delete(object_proxy &proxy) = 0;
};

}

#endif /* OBJECT_STORE_OBSERVER_HPP */
