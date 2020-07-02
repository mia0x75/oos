#include "OSTest.hpp"

#include "matador/utils/os.hpp"

using namespace matador;

OSTest::OSTest()
  : unit_test("os", "os test")
{
  add_test("mkdir", std::bind(&OSTest::test_mkdir, this), "os mkdir test");
  add_test("mkpath", std::bind(&OSTest::test_mkpath, this), "os mkpath test");
  add_test("rm", std::bind(&OSTest::test_remove_file, this), "os rm file test");
  add_test("move", std::bind(&OSTest::test_rename_file, this), "os move file test");
  add_test("access", std::bind(&OSTest::test_access_file, this), "os access file test");
}

void OSTest::test_mkdir()
{
  std::string path("temppath");
  // check if dir exists
  UNIT_ASSERT_FALSE(os::exists(path));

  // create dir
  UNIT_ASSERT_TRUE(os::mkdir(path));

  // test if dir exists
  UNIT_ASSERT_TRUE(os::exists(path));

  // delete dir
  UNIT_ASSERT_TRUE(os::rmdir(path));
}

void OSTest::test_mkpath()
{
#ifdef _WIN32
  std::string temppath("my\\tmp\\dir\\path");
#else
  std::string temppath("my/tmp/dir/path");
#endif

  // create path
  UNIT_ASSERT_TRUE(os::mkpath(temppath));

  // chdir
  UNIT_ASSERT_TRUE(os::chdir(temppath));

  UNIT_ASSERT_TRUE(os::rmpath(temppath));
}

void OSTest::test_remove_file()
{
  std::string tempfilename("temp.txt");

  // check if file exists
  UNIT_ASSERT_FALSE(os::exists(tempfilename));

  // create file
  FILE *f = os::fopen(tempfilename, "w");
  UNIT_ASSERT_NOT_NULL(f);

  // test if file exists
  UNIT_ASSERT_TRUE(os::exists(tempfilename));

  // close file
  UNIT_ASSERT_TRUE(os::fclose(f));

  // delete file
  UNIT_ASSERT_TRUE(os::remove(tempfilename));
}

void OSTest::test_rename_file()
{
  std::string tempfilename("temp.txt");

  // check if file exists
  UNIT_ASSERT_FALSE(os::exists(tempfilename));

  // create file
  FILE *f = os::fopen(tempfilename, "w");
  UNIT_ASSERT_NOT_NULL(f);

  // test if file exists
  UNIT_ASSERT_TRUE(os::exists(tempfilename));

  std::string newtempfilename("temp_new.txt");

  // check if file exists
  UNIT_ASSERT_FALSE(os::exists(newtempfilename));

  // close file
  UNIT_ASSERT_TRUE(os::fclose(f));

  // rename file
  UNIT_ASSERT_TRUE(os::rename(tempfilename, newtempfilename));

  // check if file exists
  UNIT_ASSERT_TRUE(os::exists(newtempfilename));

  // check if file exists
  UNIT_ASSERT_FALSE(os::exists(tempfilename));

  // delete file
  UNIT_ASSERT_TRUE(os::remove(newtempfilename));
}

void OSTest::test_access_file()
{
  std::string tempfilename("temp.txt");

  // check if file exists
  UNIT_ASSERT_FALSE(os::exists(tempfilename));

  // create file
  FILE *f = os::fopen(tempfilename, "w");
  UNIT_ASSERT_NOT_NULL(f);

  // test if file exists
  UNIT_ASSERT_TRUE(os::exists(tempfilename));

  // check readable
  UNIT_ASSERT_TRUE(os::is_writable(tempfilename));

  // check readable
  UNIT_ASSERT_TRUE(os::is_readable(tempfilename));

  // close file
  UNIT_ASSERT_TRUE(os::fclose(f));

  // delete file
  UNIT_ASSERT_TRUE(os::remove(tempfilename));
}
