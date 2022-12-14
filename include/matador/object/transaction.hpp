#ifndef OOS_TRANSACTION_HPP
#define OOS_TRANSACTION_HPP

#include "matador/object/action.hpp"
#include "matador/object/object_exception.hpp"
#include "matador/object/action_inserter.hpp"

#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/sequencer.hpp"

#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>

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

namespace matador {

class object_store;
class insert_action;
class update_action;
class delete_action;
/**
 * @brief This class provides the transaction mechanism
 *
 * This class provides the transaction mechanism. It can be used
 * if a couple of actions (insert, update, delete) should be made
 * at once.
 *
 * If one action fails all finished action we be rolled back. If
 * all actions could be finished one can commit the changes.
 *
 * Therefor the interface of this class provides the main methods
 * - begin
 * - commit
 * - rollback
 *
 * Transaction can be nested as well.
 */
class MATADOR_OBJECT_API transaction
{
public:
  typedef std::shared_ptr<action> action_ptr;      /**< Shortcut to an action shared pointer */
  typedef std::vector<action_ptr> t_action_vector; /**< Shortcut to a vector of action shared pointer */

public:
  /**
   * @brief Interface to an transaction observer
   *
   * The transaction observer will be notified on
   * begin, commit and rollback of a transaction
   */
  struct observer
  {
    virtual ~observer() = default;

    /**
     * @brief Interface for the begin transaction event
     */
    virtual void on_begin() = 0;

    /**
     * @fn virtual void on_commit(transaction::t_action_vector &actions)
     * @brief Interface for the commit transaction event
     *
     * The interface for the commit transaction event takes
     * a vector of all actions to be commited.
     *
     * @param actions Actions to be commited.
     */
    virtual void on_commit(transaction::t_action_vector&) = 0;

    /**
     * @brief Interface for the rollback transaction event
     */
    virtual void on_rollback() = 0;
  };

public:
  /**
   * @brief Creates a transaction for the given object_store
   *
   * @param store The object_store the transaction is created for
   */
  explicit transaction(object_store &store);

  /**
   * @brief Creates a transaction for the given object_store and observer
   *
   * @param store The object_store the transaction is created for
   * @param obsvr The observer to be used for the transaction events
   */
  transaction(object_store &store, const std::shared_ptr<observer> &obsvr);

  /**
   * @fn transaction(const transaction &x);
   * @brief Copy construct a transaction
   *
   * @param x The transaction to be copied from
   */
  transaction(const transaction&);
  transaction& operator=(const transaction&) = delete;

  /**
   * @brief Default copy move contructor for a transaction
   * @param x The transaction to be moved
   */
  transaction(transaction &&x) = default;
  /**
   * @brief Default assignment move contructor for a transaction
   * @param x The transaction to be moved
   * @return Reference to the transaction
   */
  transaction& operator=(transaction &&x) = default;

  /**
   * @brief Equal operator for two transaction objects
   *
   * Every transaction has a unique id. This id is used
   * for the comparation.
   *
   * @param a The left hand transaction to be compared
   * @param b The right hand transaction to be compared
   * @return True if transactions are the same
   */
  friend bool operator==(const transaction &a, const transaction &b);

  /**
   * @brief Unequal operator for two transaction objects
   *
   * Every transaction has a unique id. This id is used
   * for the comparation.
   *
   * @param a The left hand transaction to be compared
   * @param b The right hand transaction to be compared
   * @return True if transactions are not the same
   */
  friend bool operator!=(const transaction &a, const transaction &b);

  /**
   * @brief Returns the unique id of the transaction
   * @return The unique id of the transaction
   */
  unsigned long id() const;

  /**
   * @brief Start a new transaction
   */
  void begin();
  /**
   * @brief Finish transaction
   *
   * Finish transaction and commit all changes
   */
  void commit();

  /**
   * @brief Rollback transaction
   *
   * Rollback transaction and revert all changes
   */
  void rollback();

  /**
   * Called when insertion took place.
   * 
   * @tparam T The type of the inserted object
   */
  void on_insert(object_proxy *proxy);
  /**
   * Called when update took place.
   * 
   * @tparam T The type of the updated object
   */
  void on_update(object_proxy *proxy);
  /**
   * Called when deletion took place.
   * 
   * @tparam T The type of the deleted object
   */
  void on_delete(object_proxy *proxy);

private:
  typedef t_action_vector::iterator action_iterator;
  typedef std::unordered_map<unsigned long, t_action_vector::size_type> t_id_action_index_map;

  void backup(const action_ptr &a, const object_proxy *proxy);
  void restore(const action_ptr &a);

  void cleanup();

private:
  struct null_observer : public observer, public action_visitor
  {
    void on_begin() override {};
    void on_commit(transaction::t_action_vector &actions) override;
    void on_rollback() override {};

    void visit(insert_action *) override {}
    void visit(update_action *) override {}
    void visit(delete_action *act) override;
  };

  struct transaction_data
  {
    transaction_data(object_store &store, std::shared_ptr<observer> obsrvr)
      : store_(store)
      , inserter_(actions_)
      , observer_(std::move(obsrvr))
      , id_(transaction::sequencer_.next())
    {}

    std::reference_wrapper<object_store> store_;

    t_action_vector actions_;
    t_id_action_index_map id_action_index_map_;

    action_inserter inserter_;
    byte_buffer object_buffer_;

    std::shared_ptr<observer> observer_;

    unsigned long id_;
  };

private:
  std::shared_ptr<transaction_data> transaction_data_;
  static sequencer sequencer_;

  bool committing_ = false;
};

}
#endif //OOS_TRANSACTION_HPP
