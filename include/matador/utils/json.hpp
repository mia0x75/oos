#ifndef REACTOR_JSON_HPP
#define REACTOR_JSON_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <ostream>
#include <vector>

namespace matador {

template < class R >
void join(R &range, std::ostream &out, const std::string &delim)
{
  if (range.size() < 2) {
    for (auto &i : range) {
      out << i;
    }
  } else {
    auto it = range.begin();
    out << *it++;
    for (;it != range.end(); ++it) {
      out << delim << *it;
    }
  }
}

template < class K, class V >
void join(std::map<K, V> &range, std::ostream &out, const std::string &delim)
//void join(std::unordered_map<K, V> &range, std::ostream &out, const std::string &delim)
{
  if (range.size() < 2) {
    for (auto &i : range) {
      out << "\"" << i.first << "\": " << i.second;
    }
  } else {
    auto it = range.begin();
    out << "\"" << it->first << "\": " << it->second;
    for (++it ;it != range.end(); ++it) {
      out << delim << "\"" << it->first << "\": " << it->second;
    }
  }
}

template < class JSON_TYPE >
class json_iterator;

/**
 * @brief This class represents either a json value an object or an array
 *
 * This class represents any json value
 * - number (double or long long)
 * - string
 * - boolean
 * - object
 * - array
 *
 *
 */
class OOS_UTILS_API json
{
public:
  typedef json_iterator<json> iterator;             /**< Shortcut to json iterator type */
  typedef std::map<std::string, json> object_type;  /**< Shortcut to json object map type */
  typedef std::vector<json> array_type;             /**< Shortcut to json array vector type */

  /**
   * Enum representing the different json value types
   */
  enum json_type
  {
    e_null,     /**< Json null type */
    e_integer,  /**< Json integer type (number) */
    e_real,     /**< Json real type (number) */
    e_boolean,  /**< Json boolean type */
    e_string,   /**< Json string type */
    e_object,   /**< Json object type */
    e_array,    /**< Json array type */
  };

  /**
   * Creates an json null value
   */
  json();

  /**
   * Creates a number json value where the
   * value is treated like an integer of type long long
   *
   * @tparam T Type of the value
   * @param val Value of the json object
   */
  template<class T>
  json(T val, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0)
    : value_((long long)val), type(e_integer)
  {}

  /**
   * Creates a number json value where the
   * value is treated like a floating point of type double
   *
   * @tparam T Type of the value
   * @param val Value of the json object
   */
  template<class T>
  json(T val, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
   : value_(val), type(e_real)
  {}

  /**
   * Creates a string json value where the
   * value is treated like std::string
   *
   * @tparam T Type of the value
   * @param val Value of the json object
   */
  template<class T>
  json(T val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
   : value_(val), type(e_string)
  {}

  /**
   * Creates a boolean json value
   *
   * @param b The boolean value of the json object
   */
  json(bool b);

  /**
   * Creates a json array from the initializer list.
   *
   * Note: If the list contains just two elements and the first element
   * is of type string an json object is created.
   *
   * @param l
   */
  json(std::initializer_list<json> l);

  /**
   * Creates a null value json object
   *
   * @param n Null value
   */
  json(std::nullptr_t n);

  /**
   * Creates explicitly a json object of
   * the give json type
   *
   * @param t Type of the json object
   * @sa json_type
   */
  explicit json(json_type t);

  /**
   * Destroys the json object
   */
  ~json();

  /**
   * Creates an empty json object.
   *
   * @return An empty json object
   */
  static json object();

  /**
   * Creates an empty json array.
   *
   * @return An empty json array
   */
  static json array();

  /**
   * Assigns a value to the json object
   * On json type change the changes
   * are adjusted
   *
   * @tparam T Type of the value
   * @param val Value to assign
   * @return The assigned json object
   */
  template < class T >
  json& operator=(T val) {
    reset(val);
    return *this;
  }

  /**
   * Copy construct a josn object from
   * the given json object
   *
   * @param x The json object to copy from
   */
  json(const json &x);

  /**
   * Copy move the given json object
   *
   * @param x The json object to copy move
   */
  json(json &&x) noexcept;

  /**
   * Move assign the given json object
   *
   * @param x The json object to move assign
   * @return The move assigned json object
   */
  json& operator=(json &&x) noexcept;

  /**
   * Copy assign the given json object
   *
   * @param x The json object to copy assign
   * @return The copied json object
   */
  json& operator=(const json &x);

  /**
   * Print the json object to the given ostream
   *
   * @param out The stream to write on
   * @param val The json object to write
   * @return The written on stream
   */
  friend OOS_UTILS_API std::ostream& operator<<(std::ostream &out, const json &val);

  /**
   * Print the json object to the given ostream
   *
   * @param out The stream to write on
   */
  void dump(std::ostream &out) const;

  /**
   * Get the json object of the given key.
   * If the json type isn't object the type
   * is changed to object and a json null
   * value is inserted at key
   *
   * @param key The key of the requested value
   * @return The requested value for the given key
   */
  json& operator[](const std::string &key) {
    if (type != e_object) {
      clear();
      value_.object = new object_type;
      type = e_object;
    }
    auto it = value_.object->insert(std::make_pair(key, json())).first;
    return it->second;
  }

  /**
   * Get the json object at given index i
   * if the json if of type array.
   * 
   * If the json isn't of type array a reference
   * the this json is returned.
   * If the index is out of bound a logic_error is
   * thrown
   * 
   * @param i Index of the requested value
   * @return The requested value for the given index
   * @throws std::logic_error If the index is out of bounce
   */
  json& operator[](std::size_t i);
  const json& operator[](std::size_t i) const {
    if (type != e_array) {
      return *this;
    }
    if (i >= value_.array->size()) {
      throw std::logic_error("index out of bounce");
    }
    return value_.array->at(i);
  }

  /**
   * Push back the given value to the json array
   *
   * If the current type of the json object isn't
   * array the type is changed to array.
   *
   * @tparam T The type of the value to push back
   * @param val The value to push back
   */
  template < class T >
  void push_back(const T &val) {
    if (type != e_array) {
      clear();
      value_.array = new std::vector<json>;
      type = e_array;
    }
    value_.array->push_back(val);
  }

  /**
   * Returns a reference to the last json object
   * of the array.
   *
   * If the current type isn't array a json exception
   * is thrown.
   * @throws json_exception Throws a json_exception
   * if the current type isn't of type array
   * @return The reference of the last json object in the array
   */
  json& back();

  /**
   * Returns a const reference to the last json object
   * of the array.
   *
   * If the current type isn't array a json exception
   * is thrown.
   * @throws json_exception Throws a json_exception
   * if the current type isn't of type array
   * @return The const reference of the last json object in the array
   */
  const json& back() const;

  /**
   * Resets the json value to a new integer (number) type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  template < class T >
  void reset(T val, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0)
  {
    if (type != e_integer) {
      clear();
      type = e_integer;
    }
    value_.integer = val;
  }

  /**
   * Resets the json value to a new floating point (number) type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  template < class T >
  void reset(T val, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
  {
    if (type != e_real) {
      clear();
      type = e_real;
    }
    value_.real = val;
  }

  /**
   * Resets the json value to a new string type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  template < class T >
  void reset(T val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
  {
    if (type != e_string) {
      clear();
      type = e_string;
    }
    if (value_.str == nullptr) {
      value_.str = new std::string(val);
    } else {
      value_.str->assign(val);
    }
  }

  /**
   * Resets the json value to a new boolean type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  void reset(bool b);

  /**
   * Returns either the size of the json array or
   * the json object.
   * If the type of isn't array or object a
   * std::logic_error is thrown
   *
   * @throws std::logic_error If type isn't object or array
   * @return The size (count of elements)
   */
  std::size_t size() const;

  /**
   * Returns true if json of type array or
   * json of type object is empty. Otherwise false
   * id returned.
   * 
   * If the type of isn't array or object a
   * std::logic_error is thrown
   * 
   * @throws std::logic_error If type isn't object or array
   * @return True if array or object is empty
   */
  bool empty() const;

  /**
   * Returns the begin iterator of the json array
   * or the json object.
   * If the json isn't of type array or object this
   * json object is return in the iterator
   * @sa json_iterator
   * 
   * @return The begin iterator
   */
  iterator begin();

  /**
   * Returns the end iterator of the json array
   * or the json object.
   * If the json isn't of type array or object this
   * json object is return in the iterator
   * @sa json_iterator
   * 
   * @return The end iterator
   */
  iterator end();

  template < class T >
  typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_integer);
    return static_cast<T>(value_.integer);
  }

  template < class T >
  typename std::enable_if<std::is_floating_point<T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_real);
    return static_cast<T>(value_.real);
  }

  template < class T >
  typename std::enable_if<std::is_same<bool, T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_boolean);
    return static_cast<T>(value_.boolean);
  }

  template < class T >
  typename std::enable_if<std::is_convertible<T, std::string>::value, T>::type
  as() const {
    throw_on_wrong_type(e_string);
    return *value_.str;
  }

  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0) const {
    return is_integer();
  }

  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_floating_point<T>::value>::type * = 0) const {
    return is_real();
  }

  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_same<bool, T>::value>::type * = 0) const {
    return is_boolean();
  }

  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0) const {
    return is_string();
  }

  bool is_number() const;

  bool is_real() const;

  bool is_integer() const;

  bool is_boolean() const;

  bool is_string() const;

  bool is_array() const;

  bool is_object() const;

  bool is_null() const;

  template < class T >
  T at(std::size_t pos) const {
    if (type != e_array) {
      throw std::logic_error("type doesn't provide at()");
    }
    return value_.array->at(pos).as<T>();
  }

  void clear()
  {
    switch (type) {
      case e_string:
        delete value_.str;
        break;
      case e_object:
        delete value_.object;
        break;
      case e_array:
        delete value_.array;
        type = e_null;
        break;
      default:
        type = e_null;
        break;
    }

    value_ = {};
  }

  void erase(const std::string &key);
  void erase(std::size_t i);

private:
  void throw_on_wrong_type(json_type t) const;

  void copy_from(const json &x);

  friend class json_iterator<json>;

  union json_value {
    json_value() : integer(0) {}
    explicit json_value(long long l) : integer(l) {}
    explicit json_value(double d) : real(d) {}
    explicit json_value(bool b) : boolean(b) {}
    explicit json_value(const char *val) : str(new std::string(val)) {}
    explicit json_value(std::string s) : str(new std::string(std::move(s))) {}

    long long integer;
    double real;
    bool boolean;
    std::string *str;
    object_type *object;
    array_type *array;
  };

  json_value value_;
  json_type type = e_integer;
};

template < class JSON_TYPE >
class json_iterator
{
public:
  typedef JSON_TYPE json_type;
  typedef typename json_type::array_type array_type;
  typedef typename json_type::object_type object_type;

  json_iterator(json_type *ptr, bool as_begin)
    : obj_(ptr)
  {
    switch(obj_->type) {
      case json::e_object:
        iter.object_iterator = typename object_type::iterator();
        iter.object_iterator = (as_begin ? obj_->value_.object->begin() : obj_->value_.object->end());
        break;
      case json::e_array:
        iter.array_iterator = typename array_type::iterator();
        iter.array_iterator = (as_begin ? obj_->value_.array->begin() : obj_->value_.array->end());
        break;
      default:
        it = as_begin ? begin_ : end_;
        break;
    }
  }

  json_iterator(const json_iterator<JSON_TYPE> &x)
    : obj_(x.obj_), iter(x.iter)
  {}

  json_iterator& operator=(const json_iterator<JSON_TYPE> &x)
  {
    if (this != &x) {
      obj_ = x.obj_;
      iter = x.iter;
    }
    return *this;
  }

  /**
   * (++i)
   * @return
   */
  json_iterator operator++() {
    switch(obj_->type) {
      case json::e_object:
        ++iter.object_iterator;
        break;
      case json::e_array:
        ++iter.array_iterator;
        break;
      default:
        ++it;
        break;
    }
    return *this;
  }

  /**
   * (i++)
   * @return
   */
  json_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

    /**
     * (--i)
     * @return
     */
  json_iterator operator--() {
    switch(obj_->type) {
      case json::e_object:
        --iter.object_iterator;
        break;
      case json::e_array:
        --iter.array_iterator;
        break;
      default:
        --it;
        break;
    }
    return *this;
  }

  /**
 * (i--)
 * @return
 */
  json_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  json& operator*() const {
    switch(obj_->type) {
      case json::e_object:
        return iter.object_iterator->second;
      case json::e_array:
        return *iter.array_iterator;
      case json::e_null:
        throw std::logic_error("json null hasn't a value");
      default:
        return *obj_;
    }
  }

  json* operator->() const {
    switch(obj_->type) {
      case json::e_object:
        return &(iter.object_iterator->second);
      case json::e_array:
        return &*iter.array_iterator;
      case json::e_null:
        throw std::logic_error("json null hasn't a value");
      default:
        return obj_;
    }
  }

  bool operator==(const json_iterator<JSON_TYPE> &x) const
  {
    switch(obj_->type) {
      case json::e_object:
        return iter.object_iterator == x.iter.object_iterator;
      case json::e_array:
        return iter.array_iterator == x.iter.array_iterator;
      default:
        return it == x.it;
    }
  }

  bool operator!=(const json_iterator<JSON_TYPE> &x) const
  {
    return !operator==(x);
  }

private:
  static const std::size_t begin_ = 0;
  static const std::size_t end_ = 1;

  json *obj_;

  struct iter_type {
    typename object_type::iterator object_iterator;
    typename array_type::iterator array_iterator;
  };

  iter_type iter;
  // for builtin types (number, bool, string)
  std::size_t it = 0;
};

OOS_UTILS_API std::string to_string(const matador::json &j);

}

#endif //REACTOR_JSON_HPP
