#include "OrmReloadTestUnit.hpp"

#include "../Item.hpp"
#include "../person.hpp"
#include "../entities.hpp"
#include "../has_many_list.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

using namespace hasmanylist;

OrmReloadTestUnit::OrmReloadTestUnit(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_orm_reload", prefix + " orm reload test unit")
  , dns_(dns)
{
  add_test("load", std::bind(&OrmReloadTestUnit::test_load, this), "test load from table");
  add_test("load_twice", std::bind(&OrmReloadTestUnit::test_load_twice, this), "test load twice from table");
  add_test("load_has_one", std::bind(&OrmReloadTestUnit::test_load_has_one, this), "test load has one relation from table");
  add_test("load_has_many", std::bind(&OrmReloadTestUnit::test_load_has_many, this), "test load has many from table");
  add_test("load_has_many_to_many", std::bind(&OrmReloadTestUnit::test_load_has_many_to_many, this), "test load has many to many from table");
  //add_test("load_has_many_to_many_remove", std::bind(&OrmReloadTestUnit::test_load_has_many_to_many_remove, this), "test load has many to many from table with remove");
  add_test("load_has_many_int", std::bind(&OrmReloadTestUnit::test_load_has_many_int, this), "test load has many int from table");
  add_test("load_belongs_to_many", std::bind(&OrmReloadTestUnit::test_load_belongs_to_many, this), "test load belongs to many from table");
}

void OrmReloadTestUnit::test_load()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  std::vector<std::string> names({"hans", "otto", "georg", "hilde", "ute", "manfred"});

  {
    // insert some persons
    matador::session s(p);

    for (const std::string &name : names) {
      auto pptr = s.insert(new person(name, matador::date(18, 5, 1980), 180));

      UNIT_EXPECT_GREATER(pptr->id(), 0UL);
    }
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<person> t_person_view;
    t_person_view persons(s.store());

    UNIT_ASSERT_TRUE(!persons.empty());
    UNIT_ASSERT_EQUAL(persons.size(), 6UL);

    t_person_view::iterator first = persons.begin();
    t_person_view::iterator last = persons.end();

    while (first != last) {
      auto pptr = *first++;
      names.erase(std::remove_if(std::begin(names), std::end(names), [pptr](const std::string &name) {
        return name == pptr->name();
      }), names.end());
    }
    UNIT_ASSERT_TRUE(names.empty());
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_twice()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  std::vector<std::string> names({"hans", "otto", "georg", "hilde", "ute", "manfred"});

  {
    // insert some persons
    matador::session s(p);

    for (const std::string &name : names) {
      auto pptr = s.insert(new person(name, matador::date(18, 5, 1980), 180));

      UNIT_EXPECT_GREATER(pptr->id(), 0UL);
    }
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<person> t_person_view;
    t_person_view persons(s.store());

    UNIT_ASSERT_TRUE(!persons.empty());
    UNIT_ASSERT_EQUAL(persons.size(), 6UL);

    t_person_view::iterator first = persons.begin();
    t_person_view::iterator last = persons.end();

    while (first != last) {
      auto pptr = *first++;
      names.erase(std::remove_if(std::begin(names), std::end(names), [pptr](const std::string &name) {
        return name == pptr->name();
      }), names.end());
    }
    UNIT_ASSERT_TRUE(names.empty());
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<person> t_person_view;
    t_person_view persons(s.store());

    UNIT_ASSERT_TRUE(!persons.empty());
    UNIT_ASSERT_EQUAL(persons.size(), 6UL);

    t_person_view::iterator first = persons.begin();
    t_person_view::iterator last = persons.end();

    while (first != last) {
      auto pptr = *first++;
      names.erase(std::remove_if(std::begin(names), std::end(names), [pptr](const std::string &name) {
        return name == pptr->name();
      }), names.end());
    }
    UNIT_ASSERT_TRUE(names.empty());
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_one()
{
  matador::persistence p(dns_);

  p.attach<master>("master");
  p.attach<child>("child");

  p.create();

  {
    // insert some persons
    matador::session s(p);

    auto c = s.insert(new child("child 1"));

    s.insert(new master("master 1", c));
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<master> t_master_view;
    t_master_view masters(s.store());

    UNIT_ASSERT_TRUE(!masters.empty());
    UNIT_ASSERT_EQUAL(masters.size(), 1UL);

    auto mptr = masters.front();
    UNIT_ASSERT_NOT_NULL(mptr->children.get());

    typedef matador::object_view<child> t_child_view;
    t_child_view childview(s.store());

    UNIT_ASSERT_TRUE(!childview.empty());
    UNIT_ASSERT_EQUAL(childview.size(), 1UL);

    auto chptr = childview.front();

    UNIT_ASSERT_TRUE(chptr == mptr->children);
    UNIT_ASSERT_EQUAL(chptr.reference_count(), 1UL);
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  {
    matador::session s(p);

    auto children = s.insert(new children_list("children list 1"));

    UNIT_ASSERT_GREATER(children->id, 0UL);
    UNIT_ASSERT_TRUE(children->children.empty());

    auto kid1 = s.insert(new child("kid 1"));
    auto kid2 = s.insert(new child("kid 2"));

    UNIT_ASSERT_GREATER(kid1->id, 0UL);
    UNIT_ASSERT_GREATER(kid2->id, 0UL);

    s.push_back(children->children, kid1);
    s.push_back(children->children, kid2);

    UNIT_ASSERT_FALSE(children->children.empty());
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL);
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<children_list> t_children_list_view;
    t_children_list_view children_lists(s.store());

    UNIT_ASSERT_TRUE(!children_lists.empty());
    UNIT_ASSERT_EQUAL(children_lists.size(), 1UL);

    auto clptr = children_lists.front();

    UNIT_ASSERT_FALSE(clptr->children.empty());
    UNIT_ASSERT_EQUAL(clptr->children.size(), 2UL);

    std::vector<std::string> result_names({ "kid 1", "kid 2"});
    for (auto kid : clptr->children) {
      auto it = std::find(result_names.begin(), result_names.end(), kid->name);
      UNIT_EXPECT_FALSE(it == result_names.end());
      UNIT_ASSERT_EQUAL(kid.reference_count(), 1UL);
    }
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many_to_many()
{
  matador::persistence p(dns_);

  p.attach<person>("person");
  p.attach<student, person>("student");
  p.attach<course>("course");

  p.create();

  {
    matador::session s(p);

    auto jane = s.insert(new student("jane"));
    auto tom = s.insert(new student("tom"));
    auto art = s.insert(new course("art"));

    UNIT_ASSERT_TRUE(jane->courses.empty());
    UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL);
    UNIT_ASSERT_TRUE(tom->courses.empty());
    UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL);
    UNIT_ASSERT_TRUE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 0UL);

    s.push_back(jane->courses, art); // jane (value) must be push_back to course art (owner) students!!

    UNIT_ASSERT_FALSE(jane->courses.empty());
    UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
    UNIT_ASSERT_EQUAL(jane->courses.front(), art);
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
    UNIT_ASSERT_EQUAL(art->students.front(), jane);

    s.push_back(art->students, tom);

    UNIT_ASSERT_FALSE(tom->courses.empty());
    UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL);
    UNIT_ASSERT_EQUAL(tom->courses.front(), art);
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL);
    UNIT_ASSERT_EQUAL(art->students.back(), tom);
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<student> t_student_view;
    t_student_view student_view(s.store());

    UNIT_ASSERT_TRUE(!student_view.empty());
    UNIT_ASSERT_EQUAL(student_view.size(), 2UL);

    typedef matador::object_view<course> t_course_view;
    t_course_view course_view(s.store());

    UNIT_ASSERT_TRUE(!course_view.empty());
    UNIT_ASSERT_EQUAL(course_view.size(), 1UL);

    for (const auto &stdnt : student_view) {
      UNIT_ASSERT_FALSE(stdnt->courses.empty());
      UNIT_ASSERT_EQUAL(stdnt->courses.size(), 1UL);
    }

    auto art = course_view.front();
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL);

    auto stud = student_view.front();

    s.remove(art->students, stud);

    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many_to_many_remove()
{
  matador::persistence p(dns_);

  p.attach<person>("person");
  p.attach<student, person>("student");
  p.attach<course>("course");

  p.create();

  {
    matador::session s(p);

    auto jane = s.insert(new student("jane"));
    auto tom = s.insert(new student("tom"));
    auto art = s.insert(new course("art"));

    UNIT_ASSERT_TRUE(jane->courses.empty());
    UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL);
    UNIT_ASSERT_TRUE(tom->courses.empty());
    UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL);
    UNIT_ASSERT_TRUE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 0UL);

    s.push_back(jane->courses, art); // jane (value) must be push_back to course art (owner) students!!

    UNIT_ASSERT_FALSE(jane->courses.empty());
    UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
    UNIT_ASSERT_EQUAL(jane->courses.front(), art);
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
    UNIT_ASSERT_EQUAL(art->students.front(), jane);

    s.push_back(art->students, tom);

    UNIT_ASSERT_FALSE(tom->courses.empty());
    UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL);
    UNIT_ASSERT_EQUAL(tom->courses.front(), art);
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL);
    UNIT_ASSERT_EQUAL(art->students.back(), tom);

    s.remove(art->students, tom);

    UNIT_ASSERT_TRUE(tom->courses.empty());
    UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL);
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
    UNIT_ASSERT_EQUAL(art->students.back(), jane);

    s.push_back(art->students, tom);

    UNIT_ASSERT_FALSE(tom->courses.empty());
    UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL);
    UNIT_ASSERT_EQUAL(tom->courses.front(), art);
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL);
    UNIT_ASSERT_EQUAL(art->students.back(), tom);
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<student> t_student_view;
    t_student_view student_view(s.store());

    UNIT_ASSERT_TRUE(!student_view.empty());
    UNIT_ASSERT_EQUAL(student_view.size(), 2UL);

    typedef matador::object_view<course> t_course_view;
    t_course_view course_view(s.store());

    UNIT_ASSERT_TRUE(!course_view.empty());
    UNIT_ASSERT_EQUAL(course_view.size(), 1UL);

    for (const auto &stdnt : student_view) {
      UNIT_ASSERT_FALSE(stdnt->courses.empty());
      UNIT_ASSERT_EQUAL(stdnt->courses.size(), 1UL);
    }

    auto art = course_view.front();
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL);

    auto stud = student_view.front();

    s.remove(art->students, stud);

    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<student> t_student_view;
    t_student_view student_view(s.store());

    UNIT_ASSERT_TRUE(!student_view.empty());
    UNIT_ASSERT_EQUAL(student_view.size(), 2UL);

    typedef matador::object_view<course> t_course_view;
    t_course_view course_view(s.store());

    UNIT_ASSERT_TRUE(!course_view.empty());
    UNIT_ASSERT_EQUAL(course_view.size(), 1UL);

    auto art = course_view.front();
    UNIT_ASSERT_FALSE(art->students.empty());
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many_int()
{
  matador::persistence p(dns_);

  p.attach<many_ints>("many_ints");

  p.create();

  {
    matador::session s(p);

    auto intlist = s.insert(new many_ints);

    UNIT_ASSERT_GREATER(intlist->id, 0UL);
    UNIT_ASSERT_TRUE(intlist->elements.empty());

    s.push_back(intlist->elements, 4);

    UNIT_ASSERT_EQUAL(intlist->elements.front(), 4);
    UNIT_ASSERT_EQUAL(intlist->elements.back(), 4);

    s.push_front(intlist->elements, 7);

    UNIT_ASSERT_EQUAL(intlist->elements.front(), 7);

    UNIT_ASSERT_FALSE(intlist->elements.empty());
    UNIT_ASSERT_EQUAL(intlist->elements.size(), 2UL);
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<many_ints> t_many_ints_view;
    t_many_ints_view ints_view(s.store());

    UNIT_ASSERT_TRUE(!ints_view.empty());
    UNIT_ASSERT_EQUAL(ints_view.size(), 1UL);

    auto intlist = ints_view.front();

    UNIT_ASSERT_FALSE(intlist->elements.empty());
    UNIT_ASSERT_EQUAL(intlist->elements.size(), 2UL);

    std::vector<int> result_ints({ 4, 7 });
    for (auto i : intlist->elements) {
      auto it = std::find(result_ints.begin(), result_ints.end(), i);
      UNIT_EXPECT_FALSE(it == result_ints.end());
    }

  }

  p.drop();
}

void OrmReloadTestUnit::test_load_belongs_to_many()
{
  matador::persistence p(dns_);

  p.attach<person>("person", matador::object_store::abstract);
  p.attach<department>("department");
  p.attach<employee, person>("employee");

  p.create();

  {
    matador::session s(p);

    auto jane = s.insert(new employee("jane"));
    auto insurance = s.insert(new department("insurance"));

    UNIT_ASSERT_TRUE(insurance->employees.empty());
    UNIT_ASSERT_EQUAL(insurance.reference_count(), 0UL);
    UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL);
    UNIT_ASSERT_NULL(jane->dep().get());
    UNIT_ASSERT_EQUAL(jane.reference_count(), 0UL);

    auto tr = s.begin();
    try {
      jane->dep(insurance);
      tr.commit();
    } catch (std::exception &ex) {
      tr.rollback();
      UNIT_FAIL(ex.what());
    }

    UNIT_ASSERT_FALSE(insurance->employees.empty());
    UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
    UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
    UNIT_ASSERT_EQUAL(jane->dep(), insurance);
    UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);
  }

  p.clear();

  {
    matador::session s(p);

    s.load("employee");
    s.load<department>();

    typedef matador::object_view<employee> t_employee_view;
    t_employee_view employees(s.store());

    UNIT_ASSERT_TRUE(!employees.empty());
    UNIT_ASSERT_EQUAL(employees.size(), 1UL);

    auto jane = employees.front();

    UNIT_ASSERT_TRUE(jane->dep() != nullptr);
    UNIT_ASSERT_EQUAL(jane.reference_count(), 1UL);

    typedef matador::object_view<department> t_department_view;
    t_department_view departments(s.store());

    UNIT_ASSERT_TRUE(!departments.empty());
    UNIT_ASSERT_EQUAL(departments.size(), 1UL);

    auto insurance = departments.front();

    UNIT_ASSERT_FALSE(insurance->employees.empty());
    UNIT_ASSERT_EQUAL(insurance.reference_count(), 1UL);
    UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL);
    UNIT_ASSERT_EQUAL(insurance->employees.front(), jane);
  }

  p.drop();
}