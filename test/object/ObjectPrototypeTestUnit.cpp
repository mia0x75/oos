#include "ObjectPrototypeTestUnit.hpp"

#include "matador/object/object_store.hpp"

#include "../datatypes.hpp"

#include <iostream>

using namespace matador;
using namespace std;

ObjectPrototypeTestUnit::ObjectPrototypeTestUnit()
  : unit_test("prototype", "ObjectStore Prototype Test Unit")
{
  add_test("empty", [this] { empty_store(); }, "test empty serializable store");
  add_test("find", [this] { test_find(); }, "find prototype test");
  add_test("size", [this] { test_size(); }, "size prototype test");
  add_test("parent_of", [this] { test_is_parent_of(); }, "check parent");
  add_test("decrement", [this] { test_decrement(); }, "check decrementing iterator");
  add_test("one", [this] { one_prototype(); }, "one prototype");
  add_test("hierarchy", [this] { prototype_hierachy(); }, "prototype hierarchy");
  add_test("iterator", [this] { prototype_traverse(); }, "prototype iterator");
  add_test("relation", [this] { prototype_relation(); }, "prototype relation");
}

void
ObjectPrototypeTestUnit::empty_store()
{
  object_store ostore;

  UNIT_ASSERT_TRUE(ostore.empty());
}

void
ObjectPrototypeTestUnit::test_find()
{
  object_store ostore;
  ostore.attach<datatypes>("item");
  
  prototype_iterator i = ostore.find<datatypes>();
  
  UNIT_ASSERT_TRUE(i != ostore.end());
}

void
ObjectPrototypeTestUnit::test_size()
{
  object_store ostore;
  
  prototype_iterator i = ostore.begin();
  
  UNIT_ASSERT_TRUE(i->size() == 0);

  ostore.attach<datatypes>("item");
    
  i = ostore.find<datatypes>();

  UNIT_ASSERT_TRUE(i->size() == 0);
  
  ostore.insert(new datatypes);

  UNIT_ASSERT_TRUE(i->size() == 1);
}

void
ObjectPrototypeTestUnit::test_is_parent_of()
{
  object_store ostore;
  
  prototype_iterator root = ostore.begin();
  
  UNIT_ASSERT_TRUE(root->size() == 0);

  ostore.attach<datatypes>("item");
    
  prototype_iterator i = ostore.find<datatypes>();

  UNIT_ASSERT_FALSE(root->is_child_of(i.get()));

//  UNIT_ASSERT_TRUE(i->is_child_of(root.get()));
}

void
ObjectPrototypeTestUnit::test_decrement()
{
  object_store ostore;
  ostore.attach<datatypes>("item");
  ostore.attach<ItemA, datatypes>("item_a");

  prototype_iterator i = ostore.end();

  --i;

  UNIT_ASSERT_TRUE(--i == ostore.begin());
}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store ostore;

  ostore.attach<datatypes>("item");
  
  auto *o = ostore.create<datatypes>();
  
  UNIT_ASSERT_NOT_NULL(o);

  delete o;
  
  ostore.detach("item");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<datatypes>(), object_exception, "unknown prototype type");
}

void
ObjectPrototypeTestUnit::prototype_hierachy()
{
  object_store ostore;
  ostore.attach<datatypes>("ITEM");
  ostore.attach<ItemA, datatypes>("ITEM_A");
  ostore.attach<ItemB, datatypes>("ITEM_B");
  ostore.attach<ItemC, datatypes>("ITEM_C");

  auto *a = ostore.create<ItemB>();
  
  UNIT_ASSERT_NOT_NULL(a);
  
  delete a;
  
  ostore.detach("ITEM_B");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemB>(), object_exception, "unknown prototype type");
  
  ostore.detach("ITEM");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<datatypes>(), object_exception, "unknown prototype type");
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemA>(), object_exception, "unknown prototype type");
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemC>(), object_exception, "unknown prototype type");
}

void
ObjectPrototypeTestUnit::prototype_traverse()
{
  object_store ostore;
  ostore.attach<datatypes>("ITEM");
  ostore.attach<ItemA, datatypes>("ITEM_A");
  ostore.attach<ItemB, datatypes>("ITEM_B");
  ostore.attach<ItemC, datatypes>("ITEM_C");

  prototype_iterator first = ostore.begin();
  prototype_iterator last = ostore.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4);
    ++first;
    ++count;
  }
  
  UNIT_ASSERT_EQUAL(count, 4);
}

void
ObjectPrototypeTestUnit::prototype_relation()
{
  // Todo: complete test!
  object_store ostore;

//  ostore.attach<album>("album");
//  ostore.attach<track>("track");
//  ostore.attach<playlist>("playlist");
}
