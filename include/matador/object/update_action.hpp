#ifndef OOS_UPDATE_ACTION_HPP
#define OOS_UPDATE_ACTION_HPP

#include "matador/object/action.hpp"
#include "matador/object/delete_action.hpp"

namespace matador {

class object_proxy;
class object_serializer;

/// @cond MATADOR_DEV

/**
 * @internal
 * @class update_action
 * @brief Action when updating an serializable.
 *
 * This action is used when an objected
 * is updated on the database.
 */
class MATADOR_OBJECT_API update_action : public action
{
private:
  typedef void (*t_backup_func)(byte_buffer&, update_action*, object_serializer &serializer);
  typedef void (*t_restore_func)(byte_buffer&, update_action*, object_store*, object_serializer &serializer);

public:
  /**
   * Creates an update_action.
   *
   * @param o The updated serializable.
   */
  template < class T >
  update_action(const char* classname, unsigned long id, object_proxy *proxy, T *obj)
    : proxy_(proxy)
    , delete_action_(new delete_action(classname, id, proxy, obj))
    , backup_func_(&backup_update<T, object_serializer>)
    , restore_func_(&restore_update<T, object_serializer>)
  {}

  void accept(action_visitor *av) override;

  /**
   * The serializable of the action.
   */
  object_proxy* proxy();

  /**
   * The serializable of the action.
   */
  const object_proxy* proxy() const;

  void backup(byte_buffer &buffer) override;

  void restore(byte_buffer &buffer, object_store *store) override;

  delete_action* release_delete_action();

private:
  template < class T, class S >
  static void backup_update(byte_buffer &buffer, update_action *act, S &serializer)
  {
    T* obj = (T*)(act->proxy()->obj());
    serializer.serialize(obj, &buffer);
  }

  template < class T, class S >
  static void restore_update(byte_buffer &buffer, update_action *act, object_store *store, S &serializer)
  {
    T* obj = (T*)(act->proxy()->obj());
    serializer.deserialize(obj, &buffer, store);
  }

private:
  object_proxy *proxy_;
  std::unique_ptr<delete_action> delete_action_;

  t_backup_func backup_func_;
  t_restore_func restore_func_;
};

/// @endcond

}

#endif //OOS_UPDATE_ACTION_HPP
