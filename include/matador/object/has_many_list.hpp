#ifndef OOS_TEST_HAS_MANY_LIST_HPP
#define OOS_TEST_HAS_MANY_LIST_HPP

#include "matador/object/basic_has_many.hpp"
#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/has_many_iterator_traits.hpp"

#include "matador/utils/is_builtin.hpp"
#include "matador/utils/is_varchar.hpp"
#include "matador/utils/varchar.hpp"

#include <list>

namespace matador {

/// @cond MATADOR_DEV
template < class T, template < class ... > class C >
class has_many;
/// @endcond

/**
 * @brief Represents a has many iterator
 *
 * Represents a has many iterator for a has many
 * relationship with std::list container
 *
 * @tparam T The type of the iterator/container
 */
template < class T >
class has_many_iterator<T, std::list> : public has_many_iterator_traits<T, std::list>
{
private:
  typedef has_many_iterator_traits<T, std::list> traits;

public:
  typedef has_many_iterator<T, std::list> self;                               /**< Shortcut value self */
  typedef typename traits::value_type value_type;                             /**< Shortcut value type */
  typedef typename traits::holder_type holder_type;                           /**< Shortcut holder type */
  typedef typename traits::difference_type difference_type;                   /**< Shortcut to the difference type*/
  typedef typename traits::container_iterator container_iterator;             /**< Shortcut to the internal container iterator */
  typedef typename traits::const_container_iterator const_container_iterator; /**< Shortcut to the internal const container iterator */

public:
  /**
   * @brief Creates an empty has many iterator
   */
  has_many_iterator() = default;

  /**
   * @brief Copy constructs an iterator from another iterator
   *
   * @param iter The iterator to copy from
   */
  has_many_iterator(const self &iter) : iter_(iter.iter_) {}

  //has_many_iterator(self &&iter) = default;
  //has_many_iterator& operator=(self &&iter) = default;
  /**
   * @brief Creates a has many iterator from given internal container iterator
   *
   * @param iter The iterator to create the has many iterator from
   */
  explicit has_many_iterator(container_iterator iter) : iter_(iter) {}

  /**
   * @brief Copy assign an iterator from another iterator
   *
   * @param iter The iterator to copy from
   * @return A reference to self
   */
  has_many_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }

  ~has_many_iterator() = default;

  /**
   * @brief Compares equality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator contain
   * the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators contain the same element
   */
  bool operator==(const self &i) const { return (iter_ == i.iter_); }

  /**
   * @brief Compares unequality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator doesn't
   * contain the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators doesn't contain the same element
   */
  bool operator!=(const self &i) const { return !this->operator==(i); }

  /**
   * @brief Pre increments self
   *
   * @return A reference to incremented self
   */
  self& operator++()
  {
    ++iter_;
    return *this;
  }

  /**
   * @brief Post increments iterator
   *
   * Post increments iterator and returns a
   * new iterator object.
   *
   * @return Returns new incremented iterator
   */
  self operator++(int)
  {
    self result(*this);
    ++iter_;
    return result;
  }

  /**
   * @brief Pre decrements self
   *
   * @return A reference to decremented self
   */
  self& operator--()
  {
    --iter_;
    return *this;
  }

  /**
   * @brief Post decrements iterator
   *
   * Post decrements iterator and returns a
   * new iterator object.
   *
   * @return Returns new decremented iterator
   */
  self operator--(int)
  {
    --iter_;
    return self();
  }

  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  value_type operator->() const { return iter_->value(); }

  /**
   * Return reference of the current value
   * represented by the iterator
   *
   * @return The current value
   */
  value_type& operator*() const { return iter_->value(); }

private:
  friend class has_many<T, std::list>;
  friend class const_has_many_iterator<T, std::list>;
  friend class basic_has_many<T, std::list>;
  friend class object_serializer;
  friend class detail::object_inserter;
  friend class detail::object_deleter;

  holder_type& holder_item() const { return *iter_; }

  container_iterator iter_;
};

/**
 * @brief Represents a const has many iterator
 *
 * Represents a const has many iterator for a has many
 * relationship with std::list container
 *
 * @tparam T The type of the iterator/container
 */
template < class T >
class const_has_many_iterator<T, std::list> : public const_has_many_iterator_traits<T, std::list>
{
private:
  typedef const_has_many_iterator_traits<T, std::list> traits;

public:
  typedef const_has_many_iterator<T, std::list> self;                         /**< Shortcut to self */
  typedef typename traits::value_type value_type;                             /**< Shortcut value type */
  typedef typename traits::holder_type holder_type;                           /**< Shortcut holder type */
  typedef typename traits::difference_type difference_type;                   /**< Shortcut to the difference type */
  typedef typename traits::container_iterator container_iterator;             /**< Shortcut to the container iterator type */
  typedef typename traits::const_container_iterator const_container_iterator; /**< Shortcut to the const container iterator type */

public:
  /**
   * @brief Creates an empty const has many iterator
   */
  const_has_many_iterator() = default;

  /**
   * @brief Creates a const has many iterator from given internal container iterator
   *
   * @param iter The iterator to create the const has many iterator from
   */
  explicit const_has_many_iterator(container_iterator iter) : iter_(iter) {}

  /**
   * @brief Creates a const has many iterator from given internal const container iterator
   *
   * @param iter The const iterator to create the const has many iterator from
   */
  explicit const_has_many_iterator(const_container_iterator iter) : iter_(iter) {}

  /**
   * @brief Creates a const has many iterator from a has many iterator
   *
   * @param iter The iterator to create the const has many iterator from
   */
  const_has_many_iterator(const has_many_iterator<T, std::list> &iter) : iter_(iter.iter_) {}

  /**
   * @brief Copy construct a const_has_many_iterator from given iterator.
   *
   * @param iter Iterator to copy construct from.
   */
  const_has_many_iterator(const self &iter) : iter_(iter.iter_) {}

  /**
   * @brief Copy assigns a new const has many iterator
   *
   * @param iter The iterator to be copy assigned from
   * @return Reference to the created iterator
   */
  const_has_many_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }

  /**
   * @brief Copy assigns a new const has many iterator
   *
   * Copy assigns a new const has many iterator from
   * a non const has many iterator
   *
   * @param iter The iterator to be copy assigned from
   * @return Reference to the created iterator
   */
  const_has_many_iterator& operator=(const has_many_iterator<T, std::list> &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  ~const_has_many_iterator() = default;

  /**
   * @brief Compares equality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator contain
   * the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators contain the same element
   */
  bool operator==(const self &i) const { return (iter_ == i.iter_); }

  /**
   * @brief Compares unequality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator doesn't
   * contain the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators doesn't contain the same element
   */
  bool operator!=(const self &i) const { return !this->operator==(i); }

  /**
   * @brief Pre increments self
   *
   * @return A reference to incremented self
   */
  self& operator++()
  {
    ++iter_;
    return *this;
  }

  /**
   * @brief Post increments iterator
   *
   * Post increments iterator and returns a
   * new iterator object.
   *
   * @return Returns new incremented iterator
   */
  self operator++(int)
  {
    self tmp = *this;
    ++iter_;
    return tmp;
  }

  /**
   * @brief Pre decrements self
   *
   * @return A reference to decremented self
   */
  self& operator--()
  {
    --iter_;
    return *this;
  }

  /**
   * @brief Post decrements iterator
   *
   * Post decrements iterator and returns a
   * new iterator object.
   *
   * @return Returns new decremented iterator
   */
  self operator--(int)
  {
    --iter_;
    return self();
  }

  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  const value_type operator->() const { return iter_->value(); }

  /**
   * Return a const reference to the current value
   * represented by the iterator
   *
   * @return The current value
   */
  const value_type& operator*() const { return iter_->value(); }

private:
  friend class has_many<T, std::list>;
  friend class basic_has_many<T, std::list>;
  friend class object_serializer;
  friend class detail::object_deleter;
  friend class detail::object_inserter;

  const holder_type holder_item() const { return *iter_; }

  const_container_iterator iter_;
};

/**
 * @brief Has many relation class using a std::list as container
 *
 * The has many relation class uses a std::list as internal
 * container to store the objects.
 *
 * It provides all main interface functions std::list provides
 * - insert element at a iterator position
 * - push back an element
 * - push front an element
 * - erase an element at iterator position
 * - erase a range of elements within first and last iterator position
 * - clear the container
 *
 * All of these methods are wraps around the std::list methods plus
 * the modification in the corresponding object_store and notification
 * of the transaction observer
 *
 * The relation holds object_ptr elements as well as scalar data elements.
 *
 * @tparam T The type of the elements
 */
template < class T >
class has_many<T, std::list> : public basic_has_many<T, std::list>
{
public:

  typedef basic_has_many<T, std::list> base;                    /**< Shortcut to self */
  typedef typename base::iterator iterator;                     /**< Shortcut to iterator type */
  typedef typename base::value_type value_type;                 /**< Shortcut to value_type */
  typedef typename base::holder_type holder_type;               /**< Shortcut to holder_type */
  typedef typename base::size_type size_type;                   /**< Shortcut to size_type */

private:
  typedef typename base::container_iterator container_iterator; /**< Shortcut to container_iterator */

public:
  /**
   * @brief Creates an empty has_many object
   *
   * Creates an empty has_many object with a
   * std::list as container type
   */
  has_many() = default;

  /**
   * @brief Inserts an element at the given position.
   *
   * @param pos The position to insert at
   * @param value The element to be inserted
   * @return The iterator at position of inserted element
   */
  iterator insert(iterator pos, const value_type &value)
  {
    holder_type holder(value, nullptr);

    if (this->ostore_) {
      this->relation_info_->insert_holder(*this->ostore_, holder, this->owner_);

      this->mark_holder_as_inserted(holder);

      if (!matador::is_builtin<T>::value) {
        this->relation_info_->insert_value_into_foreign(holder, this->owner_);
      }

      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }

    return iterator(this->holder_container_.insert(pos.iter_, holder));
  }

  /**
   * @brief Inserts an element at first position.
   *
   * @param value The element to be inserted
   */
  void push_front(const value_type &value)
  {
    insert(this->begin(), value);
  }

  /**
   * @brief Inserts an element at last position.
   *
   * @param value The element to be inserted
   */
  void push_back(const value_type &value)
  {
    insert(this->end(), value);
  }

  /**
   * @brief Clears the list
   */
  void clear()
  {
    erase(this->begin(), this->end());
  }

  /**
   * Removes all values equal to given value
   * from the container
   *
   * @param value Value to remove
   */
  void remove(const value_type &value)
  {
    iterator first = this->begin();
    iterator last = this->end();
    while (first != last) {
      iterator next = first;
      ++next;
      if (*first == value) {
        erase(first);
      }
      first = next;
    }
  }

  /**
   * Removes all elements from the container for which
   * the given predicate returns true.
   *
   * @tparam P Type of the predicate
   * @param predicate Predicate to be evaluated
   */
  template < class P >
  void remove_if(P predicate)
  {
    iterator first = this->begin();
    iterator last = this->end();
    while (first != last) {
      iterator next = first;
      ++next;
      if (predicate(*first)) {
        erase(first);
      }
      first = next;
    }
  }

  /**
   * @brief Erase the element at given position.
   *
   * Erase the element at given position and return the iterator
   * following the last removed element.
   * @param i Iterator to the element to remove
   * @return Iterator following the last removed element
   */
  iterator erase(iterator i)
  {
    remove_it(i.holder_item());
    container_iterator ci = this->holder_container_.erase(i.iter_);
    return iterator(ci);
  }

  /**
   * @brief Remove the elements of given range.
   *
   * Remove the elements of the given range identified
   * by the first and last iterator position. Where the first
   * iterator is included and the last iterator is not included
   * [first; last)
   *
   * @param start First iterator of the range
   * @param end Last iterator of the range
   * @return Iterator following the last removed element
   */
  iterator erase(iterator start, iterator end)
  {
    iterator i = start;
    if (this->ostore_) {
      while (i != end) {
        if (!matador::is_builtin<T>::value) {
          this->relation_info_->remove_value_from_foreign(i.holder_item(), this->owner_);
        }
        this->relation_info_->remove_holder(*this->ostore_, i.holder_item(), this->owner_);
        ++i;
      }
      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }
    return iterator(this->holder_container_.erase(start.iter_, end.iter_));
  }

private:
  void remove_it(holder_type &holder)
  {
    if (this->ostore_) {
      if (!matador::is_builtin<T>::value) {
        this->relation_info_->remove_value_from_foreign(holder, this->owner_);
      }
      this->relation_info_->remove_holder(*this->ostore_, holder, this->owner_);
      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }
  }

  void insert_holder(const holder_type &holder)
  {
    this->mark_holder_as_inserted(const_cast<holder_type&>(holder));
    this->increment_reference_count(holder.value());
    this->holder_container_.push_back(holder);
  }

  void remove_holder(const holder_type &holder)
  {
    auto i = std::remove(this->holder_container_.begin(), this->holder_container_.end(), holder);
    this->mark_holder_as_removed(*i);
    this->decrement_reference_count(holder.value());
    this->holder_container_.erase(i, this->holder_container_.end());
  }

private:
  friend class detail::relation_endpoint_value_inserter<T>;
  friend class detail::relation_endpoint_value_remover<T>;
};

}
#endif //OOS_HAS_MANY_LIST_HPP
