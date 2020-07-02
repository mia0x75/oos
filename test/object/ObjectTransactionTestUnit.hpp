//
// Created by sascha on 3/9/16.
//

#ifndef OOS_OBJECTTRANSACTIONTESTUNIT_HPP
#define OOS_OBJECTTRANSACTIONTESTUNIT_HPP


#include "matador/unit/unit_test.hpp"

class ObjectTransactionTestUnit : public matador::unit_test
{

public:
  ObjectTransactionTestUnit();

  void test_insert();
  void test_insert_rollback();
  void test_update();
  void test_update_rollback();
  void test_delete();
  void test_delete_rollback();
  void test_nested();
  void test_nested_rollback();
  void test_foreign();
  void test_foreign_rollback();
};

#endif //OOS_OBJECTTRANSACTIONTESTUNIT_HPP
