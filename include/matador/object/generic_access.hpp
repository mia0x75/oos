#ifndef OOS_GENERIC_ACCESS_HPP
#define OOS_GENERIC_ACCESS_HPP

#include "matador/object/attribute_serializer.hpp"

#include <type_traits>

namespace matador {

/**
 * Sets a value of an object member identified by
 * the given name. The object is passed as a pointer.
 * If the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be set.
 * @tparam T     The type of the value to set.
 * @param obj    The pointer object to set the value into.
 * @param name   The name of the member variable.
 * @param val    The new value for the member.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool set(O *obj, const std::string &name, const T &val)
{
  return set(*obj, name, val);
}

/**
 * Appends a value to an object member identified by
 * the given name. The object is passed as pointer.
 * If the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be appended.
 * @tparam T     The type of the value to be appended.
 * @param obj    The object pointer to append the value to.
 * @param name   The name of the member variable.
 * @param val    The value to append.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool append(O *obj, const std::string &name, const T &val)
{
  return append(*obj, name, val);
}

/**
 * Removes a value from an object member container identified by
 * the given name. The object is passed as pointer.
 * If the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be removed.
 * @tparam T     The type of the value to be removed.
 * @param obj    The object pointer to remove the value from.
 * @param name   The name of the member variable.
 * @param val    The value to remove.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool remove(O *obj, const std::string &name, const T &val)
{
  return remove(*obj, name, val);
}

/**
 * Sets a value of an object member identified by
 * the given name. The object is passed by reference.
 * If the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be set.
 * @tparam T     The type of the value to set.
 * @param obj    The object reference to set the value into.
 * @param name   The name of the member variable.
 * @param val    The new value for the member.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool set(O &obj, const std::string &name, const T &val)
{
  attribute_reader<T> reader(name, val);
  matador::access::serialize(reader, obj);
  return reader.success();
}

/**
 * Appends a value to an object member identified by
 * the given name. The object is passed by reference.
 * If the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be appended.
 * @tparam T     The type of the value to be appended.
 * @param obj    The object reference to append the value to.
 * @param name   The name of the member variable.
 * @param val    The value to append.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool append(O &obj, const std::string &name, const T &val)
{
  has_many_attribute_reader<T> writer(name, val);
  matador::access::serialize(writer, obj);
  return writer.success();
}

/**
 * Removes a value from an object member container identified by
 * the given name. The object is passed by reference.
 * If the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be removed.
 * @tparam T     The type of the value to be removed.
 * @param obj    The object reference to remove the value from.
 * @param name   The name of the member variable.
 * @param val    The value to remove.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool remove(O &obj, const std::string &name, const T &val)
{
  has_many_attribute_writer<T> writer(name, val);
  matador::access::serialize(writer, obj);
  return writer.success();
}

/**
 * Sets string value of a member identified by
 * the given name. The value is passed as a
 * character array. If the operation succeeds
 * true is returned.
 *
 * @tparam O     The object for which the value should be set.
 * @param obj    The object to set the value into.
 * @param name   The name of the member variable.
 * @param val    Pointer to the character array containing the new string value
 * @return       True if the operation succeeds.
 */
template < typename O >
bool set(O &obj, const std::string &name, const char *val)
{
  attribute_reader<const char*> reader(name, val);
  matador::access::serialize(reader, obj);
  return reader.success();
}

/**
 * Gets the value of a member identified by
 * the given name. If the operation succeeds
 * true is returned.
 *
 * @tparam O     The object for which the value should be get.
 * @tparam T     The type of the value to retrieve.
 * @param obj    The object to get the value from.
 * @param name   The name of the member variable.
 * @param val    The reference where the value is assigned to.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool get(const O &obj, const std::string &name, T &val)
{
  attribute_writer<T> writer(name, val);
  matador::access::serialize(writer, const_cast<O&>(obj));
  return writer.success();
}

/**
 * Gets the value of a member identified by
 * the given name into a character array. If
 * the operation succeeds true is returned.
 *
 * @tparam O     The object for which the value should be get.
 * @tparam T     The type of the value to retrieve.
 * @param obj    The object to get the value from.
 * @param name   The name of the member variable.
 * @param val    The pointer to a character array.
 * @param size   The size of the character array.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool get(const O &obj, const std::string &name, char *val, size_t size)
{
  attribute_writer<T> writer(name, val, size);
  matador::access::serialize(writer, const_cast<O&>(obj));
  return writer.success();
}

/**
 * Gets the value of a member identified by
 * the given name. If the operation succeeds
 * true is returned.
 *
 * @tparam O     The object for which the value should be get.
 * @tparam T        The type of the value to retrieve.
 * @param obj    The object to get the value from.
 * @param name      The name of the member variable.
 * @param val       The reference where the value is assigned to.
 * @param precision The precision of the value to get.
 * @return          True if the operation succeeds.
 */
template < typename O, class T >
bool get(const O &obj, const std::string &name, T &val, size_t precision)
{
  attribute_writer<T> writer(name, val, precision);
  matador::access::serialize(writer, const_cast<O&>(obj));
  return writer.success();
}

}
#endif //OOS_GENERIC_ACCESS_HPP
